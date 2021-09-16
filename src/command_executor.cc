#include "xcshell/command_executor.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <iostream>
#include <map>

#include "xcshell/constants.h"
#include "xcshell/redirectelement.h"
#include "xcshell/utils.h"

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

void output_redirect(RedirectElement &redirectElement) {
  if (redirectElement.output_mode == redirectElement.output_mode::overwrite) {
    int fdout = open(redirectElement.output_redirect_file.c_str(),
                     O_WRONLY | O_TRUNC | O_CREAT, 0666);
    dup2(fdout, STDOUT_FILENO);
    close(fdout);
  } else if (redirectElement.output_mode ==
             redirectElement.output_mode::append) {
    int fdout = open(redirectElement.output_redirect_file.c_str(),
                     O_WRONLY | O_APPEND | O_CREAT, 0666);
    dup2(fdout, STDOUT_FILENO);
    close(fdout);
  }
}
int CommandExecutor::ProcessChild(RedirectElement &redirectElement) {
  // child
  struct sigaction new_action {};
  new_action.sa_handler = SIG_DFL;
  int result = sigaction(SIGINT, &new_action, nullptr);
  if (result) {
    utils::PrintSystemError(std::cerr);
  }

  auto argv = BuildArgv(redirectElement.command, redirectElement.args);
  if (redirectElement.cmd_type == redirectElement.cmd_type::direct) {
    if (!redirectElement.input_redirect_file.empty()) {
      int fdin = open(redirectElement.input_redirect_file.c_str(), O_RDONLY);
      dup2(fdin, STDIN_FILENO);
      close(fdin);
    }
    if (!redirectElement.output_redirect_file.empty()) {
      output_redirect(redirectElement);
    }
  }

  auto ret = execvp(redirectElement.command.c_str(), &argv[0]);
  // should not execute to here if success
  if (ret == ERROR_CODE_SYSTEM) {
    utils::PrintSystemError(std::cerr);
    exit(ERROR_CODE_DEFAULT);
  }
  return ERROR_CODE_DEFAULT;
}

void CommandExecutor::WaitChildExit(pid_t pid) {
  int status;
  do {
    waitpid(pid, &status, WUNTRACED);
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));
}

int CommandExecutor::Execute(const std::string &line) {
  RedirectElement redirectElement = parser.parseUserInputLine(line);
  if (build_in_.Exist(redirectElement.command)) {
    return build_in_.Execute(redirectElement.command, redirectElement.args);
  }

  pid_t pid = fork();
  if (pid == ERROR_CODE_SYSTEM) {
    // error
    utils::PrintSystemError(std::cerr);
    return ERROR_CODE_DEFAULT;
  }

  if (pid == 0) {
    return ProcessChild(redirectElement);
  } else {
    WaitChildExit(pid);
  }

  return 0;
}
