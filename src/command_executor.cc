#include "xcshell/command_executor.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>
#include <iostream>
#include <vector>

#include "xcshell/CommandParseResult.h"
#include "xcshell/constants.h"
#include "xcshell/utils.h"

void CommandExecutor::output_redirect(
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
void CommandExecutor::input_redirect(
    const CommandParseResult &command_parse_result) {
  int fd_in = open(command_parse_result.input_redirect_file.c_str(), O_RDONLY);
  dup2(fd_in, STDIN_FILENO);
  close(fd_in);
}

int CommandExecutor::ProcessChild(
    const CommandParseResult &command_parse_result, bool is_need_pipe,
    int *pipe_fds, int cmd_number) {
  // child
  ResetSignalHandlerForInterrupt();

  auto argv =
      BuildArgv(command_parse_result.command, command_parse_result.args);

  if (is_need_pipe) {
    pipe_redirect(pipe_fds, cmd_number);
  }
  if (!command_parse_result.input_redirect_file.empty()) {
    input_redirect(command_parse_result);
  }
  if (!command_parse_result.output_redirect_file.empty()) {
    output_redirect(command_parse_result);
  }

  auto ret = execvp(command_parse_result.command.c_str(), &argv[0]);
  // should not execute to here if success
  if (ret == ERROR_CODE_SYSTEM) {
    utils::PrintSystemError(std::cerr);
    exit(ERROR_CODE_DEFAULT);
  }
  return ERROR_CODE_DEFAULT;
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

int CommandExecutor::Execute(const std::string &line) {
  std::vector<CommandParseResult> command_parse_result_list =
      parser_.ParseUserInputLine(line);
  bool is_need_pipe = command_parse_result_list.size() != 1;
  int pipe_fds[2];
  pipe(pipe_fds);
  for (int i = 0; i < command_parse_result_list.size(); i++) {
    CommandParseResult command_parse_result = command_parse_result_list[i];
    if (build_in_.Exist(command_parse_result.command)) {
      return build_in_.Execute(command_parse_result.command,
                               command_parse_result.args);
    }
    pid_t pid = fork();
    if (pid == ERROR_CODE_SYSTEM) {
      // error
      utils::PrintSystemError(std::cerr);
      return ERROR_CODE_DEFAULT;
    }

    auto cmd_number = i;
    if (pid == 0) {
      return ProcessChild(command_parse_result, is_need_pipe, pipe_fds, cmd_number);
    } else {
      close(pipe_fds[1]);
      // close(pipe_fds[0]);
      WaitChildExit(pid);
    }
  }
  close(pipe_fds[0]);

  return 0;
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

void CommandExecutor::pipe_redirect(int *pipe_fds, int cmd_number) {
  if (cmd_number == 0) {
    // ouput in pipeline
    dup2(pipe_fds[1], STDOUT_FILENO);
    close(pipe_fds[0]);
    close(pipe_fds[1]);
  } else {
    // read data for pipeline
    dup2(pipe_fds[0], STDIN_FILENO);
    close(pipe_fds[1]);
    close(pipe_fds[0]);
  }
}
