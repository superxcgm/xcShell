#include "xcshell/command_executor.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <array>
#include <csignal>
#include <iostream>
#include <memory>
#include <vector>

#include "xcshell/CommandParseResult.h"
#include "xcshell/constants.h"
#include "xcshell/utils.h"

void CommandExecutor::OutputRedirect(
    const CommandParseResult &command_parse_result) {
  if (command_parse_result.output_is_append) {
    int fd_out = open(command_parse_result.output_redirect_file.c_str(),
                      O_WRONLY | O_APPEND | O_CREAT, 0664);
    dup2(fd_out, STDOUT_FILENO);
    close(fd_out);
  } else {
    int fd_out = open(command_parse_result.output_redirect_file.c_str(),
                      O_WRONLY | O_TRUNC | O_CREAT, 0664);
    dup2(fd_out, STDOUT_FILENO);
    close(fd_out);
  }
}

void CommandExecutor::InputRedirect(
    const CommandParseResult &command_parse_result) {
  int fd_in = open(command_parse_result.input_redirect_file.c_str(), O_RDONLY);
  dup2(fd_in, STDIN_FILENO);
  close(fd_in);
}

int CommandExecutor::ProcessChild(
    const CommandParseResult &command_parse_result,
    const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number,
    bool is_last_command) {
  // child
  ResetSignalHandlerForInterrupt();
  RedirectSelector(command_parse_result, pipe_fds_list, cmd_number,
                   is_last_command);
  auto argv =
      BuildArgv(command_parse_result.command, command_parse_result.args);
  auto ret = execvp(command_parse_result.command.c_str(), &argv[0]);
  // should not execute to here if success
  if (ret == ERROR_CODE_SYSTEM) {
    utils::PrintSystemError(std::cerr);
    exit(ERROR_CODE_DEFAULT);
  }
  return ERROR_CODE_DEFAULT;
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
}

void CommandExecutor::ResetSignalHandlerForInterrupt() {
  struct sigaction new_action {};
  new_action.sa_handler = SIG_DFL;
  int result = sigaction(SIGINT, &new_action, nullptr);
  if (result) {
    utils::PrintSystemError(std::cerr);
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
    if (pipe(pipe_fds) == ERROR_CODE_SYSTEM) {
      utils::PrintSystemError(std::cerr);
      exit(ERROR_CODE_DEFAULT);
    }
    pipe_fds_list.push_back({pipe_fds[0], pipe_fds[1]});
  }
  return pipe_fds_list;
}

int CommandExecutor::Execute(const std::string &line) {
  std::vector<CommandParseResult> command_parse_result_list =
      parser_.ParseUserInputLine(line);
  int save_fd = dup(STDOUT_FILENO);
  struct CommandParseResult *built_In_Command_ptr = nullptr;
  auto pipe_fds_list = CreatePipe(command_parse_result_list);
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
      pid_t pid = fork();
      if (pid == ERROR_CODE_SYSTEM) {
        return GetErrorInformation();
      }
      if (pid == 0) {
        return ProcessChild(command_parse_result_list[i], pipe_fds_list, i,
                            is_last_command);
      } else if (i + 1 == command_parse_result_list.size()) {
        // Reading end is received
        BuildInCommandExecute(save_fd, built_In_Command_ptr, pipe_fds_list);
        ProcessFather(pipe_fds_list, pid);
      }
    }
  }
  return 0;
}

int CommandExecutor::GetErrorInformation() {
  // error
  utils::PrintSystemError(std::cerr);
  return ERROR_CODE_DEFAULT;
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

void CommandExecutor::ProcessFather(
    const std::vector<std::array<int, 2>> &pipe_fds_list, pid_t pid) {
  for (auto &pipe_fds : pipe_fds_list) {
    close(pipe_fds[0]);
    close(pipe_fds[1]);
  }
  WaitChildExit(pid);
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
  dup2(pipe_fds_list[cmd_number - 1][0], STDIN_FILENO);
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
  dup2(pipe_fds_list[cmd_number - 1][0], STDIN_FILENO);
  close(pipe_fds_list[cmd_number - 1][0]);
  close(pipe_fds_list[cmd_number][0]);
  dup2(pipe_fds_list[cmd_number][1], STDOUT_FILENO);
  close(pipe_fds_list[cmd_number][1]);
}

void CommandExecutor::PipeRedirectFirst(
    const std::vector<std::array<int, 2>> &pipe_fds_list) {
  for (int i = 1; i < pipe_fds_list.size(); i++) {
    close(pipe_fds_list[i][0]);
    close(pipe_fds_list[i][1]);
  }
  close(pipe_fds_list[0][0]);
  dup2(pipe_fds_list[0][1], STDOUT_FILENO);
  close(pipe_fds_list[0][1]);
}
