#include "xcshell/command_executor.h"

#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <iostream>
#include <map>

#include "xcshell/constants.h"
#include "xcshell/parse.h"
#include "xcshell/parseTypeElement.h"
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

int CommandExecutor::ProcessChild(const std::string &command,
                                  const std::vector<std::string> &args) {
  // child
  struct sigaction new_action {};
  new_action.sa_handler = SIG_DFL;
  int result = sigaction(SIGINT, &new_action, nullptr);
  if (result) {
    utils::PrintSystemError(std::cerr);
  }

  auto argv = BuildArgv(command, args);

  auto ret = execvp(command.c_str(), &argv[0]);
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
  Parse parse;
  const auto [cmd, args] = parse.parseUserInputLine(line);
  if (build_in_.Exist(cmd)) {
    return build_in_.Execute(cmd, args);
  }

  pid_t pid = fork();
  if (pid == ERROR_CODE_SYSTEM) {
    // error
    utils::PrintSystemError(std::cerr);
    return ERROR_CODE_DEFAULT;
  }

  if (pid == 0) {
    return ProcessChild(cmd, args);
  } else {
    WaitChildExit(pid);
  }

  return 0;
}
