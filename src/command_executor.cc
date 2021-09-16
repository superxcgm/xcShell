#include "xcshell/command_executor.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <iostream>
#include <map>

#include "xcshell/CommandParseResult.h"
#include "xcshell/constants.h"
#include "xcshell/utils.h"

void output_redirect(const CommandParseResult &commandParseResult) {
  if (commandParseResult.output_is_append) {
    int fdout = open(commandParseResult.output_redirect_file.c_str(),
                     O_WRONLY | O_APPEND | O_CREAT, 0664);
    dup2(fdout, STDOUT_FILENO);
    close(fdout);
  } else {
    int fdout = open(commandParseResult.output_redirect_file.c_str(),
                     O_WRONLY | O_TRUNC | O_CREAT, 0664);
    dup2(fdout, STDOUT_FILENO);
    close(fdout);
  }
}
void input_redirect(const CommandParseResult &commandParseResult) {
  int fdin = open(commandParseResult.input_redirect_file.c_str(), O_RDONLY);
  dup2(fdin, STDIN_FILENO);
  close(fdin);
}

int CommandExecutor::ProcessChild(
    const CommandParseResult &commandParseResult) {
  // child
  struct sigaction new_action {};
  new_action.sa_handler = SIG_DFL;
  int result = sigaction(SIGINT, &new_action, nullptr);
  if (result) {
    utils::PrintSystemError(std::cerr);
  }

  auto argv =
      Parser::BuildArgv(commandParseResult.command, commandParseResult.args);

  if (!commandParseResult.input_redirect_file.empty()) {
    input_redirect(commandParseResult);
  }
  if (!commandParseResult.output_redirect_file.empty()) {
    output_redirect(commandParseResult);
  }

  auto ret = execvp(commandParseResult.command.c_str(), &argv[0]);
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
  CommandParseResult commandParseResult = parser_.ParseUserInputLine(line);
  if (build_in_.Exist(commandParseResult.command)) {
    return build_in_.Execute(commandParseResult.command,
                             commandParseResult.args);
  }

  pid_t pid = fork();
  if (pid == ERROR_CODE_SYSTEM) {
    // error
    utils::PrintSystemError(std::cerr);
    return ERROR_CODE_DEFAULT;
  }

  if (pid == 0) {
    return ProcessChild(commandParseResult);
  } else {
    WaitChildExit(pid);
  }

  return 0;
}
