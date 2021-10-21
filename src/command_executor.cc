#include "xcshell/command_executor.h"

#include <fcntl.h>
#include <spdlog/spdlog.h>
#include <sys/wait.h>
#include <unistd.h>

#include <array>
#include <csignal>
#include <iostream>
#include <memory>
#include <vector>

#include "xcshell/command_parse_result.h"
#include "xcshell/constants.h"
#include "xcshell/error_handling.h"
#include "xcshell/utils.h"

int CommandExecutor::GetFileDescriptor(const std::string &file_name,
                                       bool is_append) {
  int fd;
  if (is_append) {
    fd = error_handling_.ErrorDispatchHandler(
        open(file_name.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0664),
        ErrorType::FATAL_ERROR);
  } else {
    fd = error_handling_.ErrorDispatchHandler(
        open(file_name.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0664),
        ErrorType::FATAL_ERROR);
  }
  return fd;
}

void CommandExecutor::OutputRedirect(
    const CommandParseResult &command_parse_result) {
  int fd_out = GetFileDescriptor(command_parse_result.output_redirect_file,
                                 command_parse_result.output_is_append);
  error_handling_.ErrorDispatchHandler(dup2(fd_out, STDOUT_FILENO),
                                       ErrorType::FATAL_ERROR);
  close(fd_out);
}

void CommandExecutor::ErrorOutputRedirect(
    const CommandParseResult &command_parse_result) {
  int fd_err = GetFileDescriptor(command_parse_result.error_redirect_file,
                                 command_parse_result.stderr_is_append);
  error_handling_.ErrorDispatchHandler(dup2(fd_err, STDERR_FILENO),
                                       ErrorType::FATAL_ERROR);
  close(fd_err);
}

void CommandExecutor::InputRedirect(
    const CommandParseResult &command_parse_result) {
  int fd_in = error_handling_.ErrorDispatchHandler(
      open(command_parse_result.input_redirect_file.c_str(), O_RDONLY),
      ErrorType::FATAL_ERROR);
  error_handling_.ErrorDispatchHandler(dup2(fd_in, STDIN_FILENO),
                                       ErrorType::FATAL_ERROR);
  close(fd_in);
}

int CommandExecutor::ProcessChild(
    const CommandParseResult &command_parse_result,
    const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number,
    bool is_last_command) {
  ResetSignalHandlerForInterrupt();
  RedirectSelector(command_parse_result, pipe_fds_list, cmd_number,
                   is_last_command);
  auto argv =
      BuildArgv(command_parse_result.command, command_parse_result.args);
  error_handling_.ErrorDispatchHandler(
      execvp(command_parse_result.command.c_str(), &argv[0]),
      ErrorType::NORMAL_ERROR);
  _exit(ERROR_CODE_DEFAULT);
}

void CommandExecutor::RedirectSelector(
    const CommandParseResult &command_parse_result,
    const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number,
    bool is_last_command) {
  if (!pipe_fds_list.empty()) {
    PipeRedirect(pipe_fds_list, cmd_number, is_last_command);
  }
  if (!command_parse_result.input_redirect_file.empty()) {
    InputRedirect(command_parse_result);
  }
  if (!command_parse_result.output_redirect_file.empty()) {
    OutputRedirect(command_parse_result);
  }
  if (!command_parse_result.error_redirect_file.empty()) {
    ErrorOutputRedirect(command_parse_result);
  }
}

void CommandExecutor::ResetSignalHandlerForInterrupt() {
  struct sigaction new_action {};
  new_action.sa_handler = SIG_DFL;
  int result = sigaction(SIGINT, &new_action, nullptr);
  if (result) {
    error_handling_.PrintSystemError(std::cerr);
  }
}

void CommandExecutor::WaitChildExit(pid_t pid) {
  int status;
  do {
    waitpid(pid, &status, WUNTRACED);
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));
}

std::vector<std::array<int, 2>> CommandExecutor::CreatePipe(
    const std::vector<CommandParseResult> &command_parse_result_list) {
  std::vector<std::array<int, 2>> pipe_fds_list;
  auto pipe_number = command_parse_result_list.size() - 1;
  for (int i = 0; i < pipe_number; i++) {
    int pipe_fds[2];
    error_handling_.ErrorDispatchHandler(pipe(pipe_fds),
                                         ErrorType::FATAL_ERROR);
    pipe_fds_list.push_back({pipe_fds[0], pipe_fds[1]});
  }
  return pipe_fds_list;
}

int CommandExecutor::Execute(const std::string &line) {
  std::vector<CommandParseResult> command_parse_result_list =
      parser_.ParseUserInputLine(line);
  LogCommandParseResultList(command_parse_result_list);
  int save_fd = error_handling_.ErrorDispatchHandler(dup(STDOUT_FILENO),
                                                     ErrorType::FATAL_ERROR);
  struct CommandParseResult *built_In_Command_ptr = nullptr;
  std::vector<std::array<int, 2>> pipe_fds_list;
  if (command_parse_result_list.size() > 1) {
    pipe_fds_list = CreatePipe(command_parse_result_list);
  }
  std::vector<pid_t> child_pids;
  for (int i = 0; i < command_parse_result_list.size(); i++) {
    bool is_last_command = i == command_parse_result_list.size() - 1;
    if (build_in_.Exist(command_parse_result_list[i].command)) {
      // build_in command pipe redirect,need the reader needs to be received
      // before it can be written to the pipe
      if (!pipe_fds_list.empty()) {
        built_In_Command_ptr = &command_parse_result_list[i];
      } else {
        build_in_.Execute(command_parse_result_list[i].command,
                          command_parse_result_list[i].args);
      }
    } else {
      pid_t pid =
          error_handling_.ErrorDispatchHandler(fork(), ErrorType::FATAL_ERROR);
      if (pid == 0) {
        close(save_fd);
        return ProcessChild(command_parse_result_list[i], pipe_fds_list, i,
                            is_last_command);
      } else {
        child_pids.push_back(pid);
      }
    }
  }
  spdlog::debug("Father wait on last command.");
  // Reading end is received
  BuildInCommandExecute(save_fd, built_In_Command_ptr, pipe_fds_list);
  CloseAllPipeAndWaitChildProcess(pipe_fds_list, child_pids);
  close(save_fd);
  return 0;
}

void CommandExecutor::BuildInCommandExecute(
    int save_fd, CommandParseResult *built_In_Command_ptr,
    const std::vector<std::array<int, 2>> &pipe_fds_list) {
  if (built_In_Command_ptr != nullptr) {
    PipeRedirectFirst(pipe_fds_list);
    build_in_.Execute(built_In_Command_ptr->command,
                      built_In_Command_ptr->args);
    built_In_Command_ptr = nullptr;
    dup2(save_fd, STDOUT_FILENO);
  }
}

void CommandExecutor::CloseAllPipeAndWaitChildProcess(
    const std::vector<std::array<int, 2>> &pipe_fds_list,
    const std::vector<pid_t> &child_pids) {
  for (auto &pipe_fds : pipe_fds_list) {
    close(pipe_fds[0]);
    close(pipe_fds[1]);
  }
  for (auto pid : child_pids) {
    WaitChildExit(pid);
  }
}

std::vector<char *> CommandExecutor::BuildArgv(
    const std::string &command, const std::vector<std::string> &args) {
  std::vector<char *> argv;
  argv.reserve(args.size() + 2);
  argv.push_back(const_cast<char *>(command.c_str()));
  for (const auto &arg : args) {
    argv.push_back(const_cast<char *>(arg.c_str()));
  }
  argv.push_back(nullptr);
  return argv;
}

void CommandExecutor::PipeRedirect(
    const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number,
    bool is_last_command) {
  if (cmd_number == 0) {
    PipeRedirectFirst(pipe_fds_list);
  }
  if (cmd_number > 0 && !is_last_command) {
    PipeRedirectMiddle(pipe_fds_list, cmd_number);
  }
  if (is_last_command) {
    PipeRedirectEnd(pipe_fds_list, cmd_number);
  }
}

void CommandExecutor::PipeRedirectEnd(
    const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number) {
  for (int i = 0; i < pipe_fds_list.size() - 1; i++) {
    close(pipe_fds_list[i][0]);
    close(pipe_fds_list[i][1]);
  }
  close(pipe_fds_list[cmd_number - 1][1]);
  error_handling_.ErrorDispatchHandler(
      dup2(pipe_fds_list[cmd_number - 1][0], STDIN_FILENO),
      ErrorType::FATAL_ERROR);
  close(pipe_fds_list[cmd_number - 1][0]);
}

void CommandExecutor::PipeRedirectMiddle(
    const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number) {
  for (int i = 0; i < pipe_fds_list.size(); i++) {
    if (i != cmd_number - 1 && i != cmd_number) {
      close(pipe_fds_list[i][0]);
      close(pipe_fds_list[i][1]);
    }
  }
  close(pipe_fds_list[cmd_number - 1][1]);
  error_handling_.ErrorDispatchHandler(
      dup2(pipe_fds_list[cmd_number - 1][0], STDIN_FILENO),
      ErrorType::FATAL_ERROR);
  close(pipe_fds_list[cmd_number - 1][0]);
  close(pipe_fds_list[cmd_number][0]);
  error_handling_.ErrorDispatchHandler(
      dup2(pipe_fds_list[cmd_number][1], STDOUT_FILENO),
      ErrorType::FATAL_ERROR);
  close(pipe_fds_list[cmd_number][1]);
}

void CommandExecutor::PipeRedirectFirst(
    const std::vector<std::array<int, 2>> &pipe_fds_list) {
  for (int i = 1; i < pipe_fds_list.size(); i++) {
    close(pipe_fds_list[i][0]);
    close(pipe_fds_list[i][1]);
  }
  close(pipe_fds_list[0][0]);
  error_handling_.ErrorDispatchHandler(dup2(pipe_fds_list[0][1], STDOUT_FILENO),
                                       ErrorType::FATAL_ERROR);
  close(pipe_fds_list[0][1]);
}
void CommandExecutor::LogCommandParseResultList(
    const std::vector<CommandParseResult> &command_parse_result_list) {
  spdlog::debug("Command parse result list, size: {}",
                command_parse_result_list.size());
  for (int i = 0; i < command_parse_result_list.size(); i++) {
    std::string args;
    for (auto &arg : command_parse_result_list[i].args) {
      args.append(arg + ", ");
    }
    spdlog::debug("Command parse result list: {}, command: {}, args: {}", i,
                  command_parse_result_list[i].command, args);
  }
}
