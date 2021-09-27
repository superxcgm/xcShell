#include "xcshell/command_executor.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

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
    const CommandParseResult &command_parse_result, bool is_pipe_redirect,
    std::vector<int *> &pipe_fds_list, int cmd_number, int cmd_last_number) {
  // child
  ResetSignalHandlerForInterrupt();
  RedirectSelector(command_parse_result, is_pipe_redirect, pipe_fds_list,
                   cmd_number, cmd_last_number);
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
    const CommandParseResult &command_parse_result, bool is_pipe_redirect,
    std::vector<int *> &pipe_fds_list, int cmd_number, int cmd_last_number) {
  if (!pipe_fds_list.empty()) {
    PipeRedirect(pipe_fds_list, cmd_number, cmd_last_number);
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

int CommandExecutor::Execute(const std::string &line) {
  std::vector<CommandParseResult> command_parse_result_list =
      parser_.ParseUserInputLine(line);
  int save_fd = dup(STDOUT_FILENO);
  bool is_pipe_redirect = command_parse_result_list.size() > 1;
  std::shared_ptr<CommandParseResult> built_In_Command_ptr = nullptr;

  auto pipe_number = command_parse_result_list.size() - 1;
  std::vector<int *> pipe_fds_list;
  for (int i = 0; i < pipe_number; i++) {
    int *pipe_fds = new int[2];
    if (pipe(pipe_fds) == -1) {
      utils::PrintSystemError(std::cerr);
      return ERROR_CODE_DEFAULT;
    }
    pipe_fds_list.push_back(pipe_fds);
  }

  for (int i = 0; i < command_parse_result_list.size(); i++) {
    auto cmd_number = i;
    int cmd_last_number = command_parse_result_list.size();
    const CommandParseResult command_parse_result =
        command_parse_result_list[i];
    if (build_in_.Exist(command_parse_result.command)) {
      // build_in command pipe redirect,need the reader needs to be received
      // before it can be written to the pipe
      if (!pipe_fds_list.empty()) {
        built_In_Command_ptr =
            std::make_shared<CommandParseResult>(command_parse_result_list[i]);
      } else {
        build_in_.Execute(command_parse_result.command,
                          command_parse_result.args);
      }
    } else {
      pid_t pid = fork();
      if (pid == ERROR_CODE_SYSTEM) {
        // error
        utils::PrintSystemError(std::cerr);
        return ERROR_CODE_DEFAULT;
      }

      if (pid == 0) {
        return ProcessChild(command_parse_result, is_pipe_redirect,
                            pipe_fds_list, cmd_number, cmd_last_number);
      } else {
        // Reading end is received
        built_In_Command_ptr = BuildInCommandPipeExecute(
            save_fd, built_In_Command_ptr, pipe_fds_list,
            command_parse_result_list, cmd_number);
        ProcessFather(command_parse_result_list, pipe_fds_list, cmd_number,
                      pid);
      }
    }
  }
  return 0;
}

void CommandExecutor::ProcessFather(
    const std::vector<CommandParseResult> &command_parse_result_list,
    std::vector<int *> &pipe_fds_list, int cmd_number, pid_t pid) {
  if (cmd_number + 1 == command_parse_result_list.size()) {
    for (auto &pipe_fds : pipe_fds_list) {
      close(pipe_fds[0]);
      close(pipe_fds[1]);
    }
    WaitChildExit(pid);
  }
}

std::shared_ptr<CommandParseResult> CommandExecutor::BuildInCommandPipeExecute(
    int save_fd, std::shared_ptr<CommandParseResult> built_In_Command_ptr,
    std::vector<int *> &pipe_fds_list,
    const std::vector<CommandParseResult> &command_parse_result_list,
    int cmd_number) {
  if (built_In_Command_ptr != nullptr &&
      cmd_number + 1 == command_parse_result_list.size()) {
    for (int i = 1; i < pipe_fds_list.size(); i++) {
      close(pipe_fds_list[i][0]);
      close(pipe_fds_list[i][1]);
    }
    close(pipe_fds_list[0][0]);
    dup2(pipe_fds_list[0][1], STDOUT_FILENO);
    close(pipe_fds_list[0][1]);
    build_in_.Execute(built_In_Command_ptr->command,
                      built_In_Command_ptr->args);
    built_In_Command_ptr = nullptr;
    dup2(save_fd, STDOUT_FILENO);
  }
  return built_In_Command_ptr;
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

void CommandExecutor::PipeRedirect(std::vector<int *> &pipe_fds_list,
                                   int cmd_number, int cmd_sum_number) {
  if (cmd_number == 0) {
    for (int i = 1; i < pipe_fds_list.size(); i++) {
      close(pipe_fds_list[i][0]);
      close(pipe_fds_list[i][1]);
    }
    close(pipe_fds_list[0][0]);
    dup2(pipe_fds_list[0][1], STDOUT_FILENO);
    close(pipe_fds_list[0][1]);
  }
  if (cmd_number > 0 && cmd_number < cmd_sum_number - 1) {
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
  if (cmd_number == cmd_sum_number - 1) {
    for (int i = 0; i < pipe_fds_list.size() - 1; i++) {
      close(pipe_fds_list[i][0]);
      close(pipe_fds_list[i][1]);
    }
    close(pipe_fds_list[cmd_number - 1][1]);
    dup2(pipe_fds_list[cmd_number - 1][0], STDIN_FILENO);
    close(pipe_fds_list[cmd_number - 1][0]);
  }
}
