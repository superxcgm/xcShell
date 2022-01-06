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

int CommandExecutor::GetFileDescriptor(const std::string &file_name,
                                       bool is_append) {
  int fd;
  if (is_append) {
    fd = ErrorHandling::ErrorDispatchHandler(
        open(file_name.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0660),
        ErrorHandling::ErrorType::FATAL_ERROR);
  } else {
    fd = ErrorHandling::ErrorDispatchHandler(
        open(file_name.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0660),
        ErrorHandling::ErrorType::FATAL_ERROR);
  }
  return fd;
}

void CommandExecutor::OutputRedirect(
    const CommandParseResult &command_parse_result) {
  int fd_out = GetFileDescriptor(command_parse_result.output_redirect_file,
                                 command_parse_result.output_is_append);
  ErrorHandling::ErrorDispatchHandler(dup2(fd_out, STDOUT_FILENO),
                                      ErrorHandling::ErrorType::FATAL_ERROR);
  close(fd_out);
}

void CommandExecutor::ErrorOutputRedirect(
    const CommandParseResult &command_parse_result) {
  int fd_err = GetFileDescriptor(command_parse_result.error_redirect_file,
                                 command_parse_result.stderr_is_append);
  ErrorHandling::ErrorDispatchHandler(dup2(fd_err, STDERR_FILENO),
                                      ErrorHandling::ErrorType::FATAL_ERROR);
  close(fd_err);
}

void CommandExecutor::InputRedirect(
    const CommandParseResult &command_parse_result) {
  int fd_in = ErrorHandling::ErrorDispatchHandler(
      open(command_parse_result.input_redirect_file.c_str(), O_RDONLY),
      ErrorHandling::ErrorType::FATAL_ERROR);
  ErrorHandling::ErrorDispatchHandler(dup2(fd_in, STDIN_FILENO),
                                      ErrorHandling::ErrorType::FATAL_ERROR);
  close(fd_in);
}

int CommandExecutor::ProcessChild(
    CommandParseResult &command_parse_result,
    const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number,
    bool is_last_command) {
  ResetSignalHandlerForInterrupt();
  RedirectSelector(command_parse_result, pipe_fds_list, cmd_number,
                   is_last_command);
  const int max_args = 60;
  std::array<char *, max_args> argv{};
  argv[0] = command_parse_result.command.data();
  int i;
  for (i = 0; i < command_parse_result.args.size(); ++i) {
    argv[i + 1] = command_parse_result.args[i].data();
  }
  argv[i + 1] = nullptr;

  ErrorHandling::ErrorDispatchHandler(
      execvp(command_parse_result.command.c_str(), argv.data()),
      ErrorHandling::ErrorType::NORMAL_ERROR);
  _exit(error_code_default);
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
    ErrorHandling::PrintSystemError();
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
    std::array<int, 2> pipe_fds{};
    ErrorHandling::ErrorDispatchHandler(pipe(pipe_fds.data()),
                                        ErrorHandling::ErrorType::FATAL_ERROR);
    pipe_fds_list.push_back({pipe_fds[0], pipe_fds[1]});
  }
  return pipe_fds_list;
}

int CommandExecutor::Execute(const std::string &line) {
  std::optional<std::vector<CommandParseResult>> maybe_commands =
      parser_.Parse(line);
  if (!maybe_commands.has_value()) {
    return error_code_default;
  }
  auto commands = maybe_commands.value();
  LogCommandParseResultList(commands);
  int save_fd = ErrorHandling::ErrorDispatchHandler(
      dup(STDOUT_FILENO), ErrorHandling::ErrorType::FATAL_ERROR);
  CommandParseResult *built_In_Command_ptr = nullptr;
  std::vector<std::array<int, 2>> pipe_fds_list;
  if (commands.size() > 1) {
    pipe_fds_list = CreatePipe(commands);
  }
  std::vector<pid_t> child_pids;
  for (int i = 0; i < commands.size(); i++) {
    bool is_last_command = i == commands.size() - 1;
    if (build_in_.Exist(commands[i].command)) {
      // build_in command pipe redirect,need the reader needs to be received
      // before it can be written to the pipe
      if (!pipe_fds_list.empty()) {
        built_In_Command_ptr = &commands[i];
      } else {
        build_in_.Execute(commands[i].command, commands[i].args);
      }
    } else {
      pid_t pid = ErrorHandling::ErrorDispatchHandler(
          fork(), ErrorHandling::ErrorType::FATAL_ERROR);
      if (pid == 0) {
        close(save_fd);
        return ProcessChild(commands[i], pipe_fds_list, i, is_last_command);
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
  return success;
}

void CommandExecutor::BuildInCommandExecute(
    int save_fd, const CommandParseResult *built_In_Command_ptr,
    const std::vector<std::array<int, 2>> &pipe_fds_list) {
  if (built_In_Command_ptr != nullptr) {
    PipeRedirectFirst(pipe_fds_list);
    build_in_.Execute(built_In_Command_ptr->command,
                      built_In_Command_ptr->args);
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
  ErrorHandling::ErrorDispatchHandler(
      dup2(pipe_fds_list[cmd_number - 1][0], STDIN_FILENO),
      ErrorHandling::ErrorType::FATAL_ERROR);
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
  ErrorHandling::ErrorDispatchHandler(
      dup2(pipe_fds_list[cmd_number - 1][0], STDIN_FILENO),
      ErrorHandling::ErrorType::FATAL_ERROR);
  close(pipe_fds_list[cmd_number - 1][0]);
  close(pipe_fds_list[cmd_number][0]);
  ErrorHandling::ErrorDispatchHandler(
      dup2(pipe_fds_list[cmd_number][1], STDOUT_FILENO),
      ErrorHandling::ErrorType::FATAL_ERROR);
  close(pipe_fds_list[cmd_number][1]);
}

void CommandExecutor::PipeRedirectFirst(
    const std::vector<std::array<int, 2>> &pipe_fds_list) {
  for (int i = 1; i < pipe_fds_list.size(); i++) {
    close(pipe_fds_list[i][0]);
    close(pipe_fds_list[i][1]);
  }
  close(pipe_fds_list[0][0]);
  ErrorHandling::ErrorDispatchHandler(dup2(pipe_fds_list[0][1], STDOUT_FILENO),
                                      ErrorHandling::ErrorType::FATAL_ERROR);
  close(pipe_fds_list[0][1]);
}
void CommandExecutor::LogCommandParseResultList(
    const std::vector<CommandParseResult> &command_parse_result_list) const {
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
