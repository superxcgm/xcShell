#include "../include/xcshell/command_executor.h"

#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <map>

#include "../include/xcshell/error.h"
#include "../include/xcshell/utils.h"

int CommandExecutor::Execute(const std::string &command,
                             const std::vector<std::string> &args) {
  if (build_in_.Exist(command)) {
    return build_in_.Execute(command, args);
  }

  pid_t pid = fork();
  if (pid == ERROR_CODE_SYSTEM) {
    // error
    utils::PrintSystemError();
    return ERROR_CODE_DEFAULT;
  }

  if (pid == 0) {
    return ProcessChild(command, args);
  } else {
    WaitChildExit(pid);
  }

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

int CommandExecutor::ProcessChild(const std::string &command,
                                  const std::vector<std::string> &args) {
  // child
  auto argv = BuildArgv(command, args);

  auto ret = execvp(command.c_str(), &argv[0]);
  // should not execute to here if success
  if (ret == ERROR_CODE_SYSTEM) {
    utils::PrintSystemError();
  }
  return ERROR_CODE_DEFAULT;
}

void CommandExecutor::WaitChildExit(pid_t pid) {
  int status;
  do {
    waitpid(pid, &status, WUNTRACED);
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));
}
