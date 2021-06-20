#include "../include/command_executor.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "../include/xc_error.h"

int CommandExecutor::Execute(const std::string &command,
                             const std::vector<std::string> &args,
                             std::ostream &err_os) {
  pid_t pid = fork();
  if (pid == ERROR_CODE_SYSTEM) {
    // error
    PrintSystemError(err_os);
    return ERROR_CODE_DEFAULT;
  }

  if (pid == 0) {
    return ProcessChild(command, args, err_os);
  } else {
    WaitChildExit(pid);
  }

  return 0;
}

void CommandExecutor::PrintSystemError(std::ostream &err_os) {
  std::string err_msg = strerror(errno);
  err_os << err_msg << std::endl;
}

std::vector<char *> CommandExecutor::BuildArgv(
    const std::string command,
    const std::vector<std::string> &args) {
  std::vector<char *> argv;
  argv.reserve(args.size() + 2);
  argv.push_back(const_cast<char *>(command.c_str()));
  for (const auto &arg : args) {
    argv.push_back(const_cast<char *>(arg.c_str()));
  }
  argv.push_back(nullptr);
  return argv;
}

int CommandExecutor::ProcessChild(const std::string &command,
                                  const std::vector<std::string> &args,
                                  std::ostream &err_os) {
  // child
  auto argv = BuildArgv(command, args);

  auto ret = execvp(command.c_str(), &argv[0]);
  // should not execute to here is success
  if (ret == ERROR_CODE_SYSTEM) {
    PrintSystemError(err_os);
  }
  return ERROR_CODE_DEFAULT;
}

void CommandExecutor::WaitChildExit(pid_t pid) {
  int status;
  do {
    waitpid(pid, &status, WUNTRACED);
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));
}
