#include "../include/xcshell/command_executor.h"

#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <map>

#include "../include/xcshell/error.h"
#include "../include/xcshell/utils.h"

int ShellCd(std::vector<std::string> args, std::ostream &err_os) {
  static std::string pre;
  if (args.size() > 1) {
    err_os << "invalid args" << std::endl;
    return ERROR_CODE_DEFAULT;
  }

  std::string path = args.empty() ? "~" : args[0];
  if (path == "~") {
    path = utils::GetHomeDir();
  } else if (path == "-") {
    path = pre;
    // cover when pre is empty
    if (path.empty()) {
      path = ".";
    }
  }

  pre = utils::GetCurrentWorkingDirectory(err_os);

  int ret = chdir(path.c_str());
  if (ret == ERROR_CODE_SYSTEM) {
    PrintSystemError(err_os);
    return ERROR_CODE_DEFAULT;
  }
  return 0;
}

std::map<std::string, int (*)(std::vector<std::string>, std::ostream &)> mp = {
    {"cd", ShellCd}};

int CommandExecutor::Execute(const std::string &command,
                             const std::vector<std::string> &args,
                             std::ostream &os, std::ostream &err_os) {
  if (mp.find(command) != mp.end()) {
    auto fun = mp[command];
    return fun(args, err_os);
  }

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
                                  const std::vector<std::string> &args,
                                  std::ostream &err_os) {
  // child
  auto argv = BuildArgv(command, args);

  auto ret = execvp(command.c_str(), &argv[0]);
  // should not execute to here if success
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
