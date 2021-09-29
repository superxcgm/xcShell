#if defined(__linux__)
#include <readline/history.h>
#include <readline/readline.h>
#elif defined(__APPLE__)
#include <editline/readline.h>
#endif

#include <csignal>
#include <sstream>

#include "xcshell/shell.h"
#include "xcshell/constants.h"
#include "xcshell/utils.h"

void Shell::Init() {
  auto global_config = utils::ReadFileText(GLOBAL_CONFIG_FILE);
  ExecuteConfig(global_config);

  auto user_config = utils::ReadFileText(utils::ExpandPath(USER_CONFIG_FILE));
  ExecuteConfig(user_config);

  IgnoreSignalInterrupt();
}

void Shell::Process() {
  while (true) {
    char *line_ptr;
    line_ptr = readline(GeneratePrompt().c_str());
    if (line_ptr == nullptr) {
      break;
    }
    if (strlen(line_ptr) > 0) {
      add_history(line_ptr);
    }
    std::string line = line_ptr;
    free(line_ptr);
    if (line.empty()) {
      continue;
    }
    command_executor_.Execute(line);
  }
}

int Shell::Exit() {
  // todo: do necessary clean up
  return 0;
}

std::string Shell::GeneratePrompt() {
  auto pwd = utils::GetCurrentWorkingDirectory(std::cerr);
  auto home = utils::GetHomeDir();
  std::string dir;
  if (home == pwd) {
    dir = "~";
  } else {
    dir = utils::GetLastDir(pwd);
  }

  auto branch_name = utils::GetCommandExecuteResult(
      R"(git branch | grep "^\*" | sed 's/^..//')");
  auto is_git_manage = utils::GetCommandExecuteResult(
                           "git rev-parse --is-inside-work-tree") == "true";

  if (!branch_name.empty() && branch_name != ".git" && is_git_manage) {
    dir += "\033[31m git(" + branch_name + ")\033[0m ";
  }

  return dir + " > ";
}

void Shell::ExecuteConfig(const std::string &config_string) {
  if (config_string.empty()) {
    return;
  }
  std::istringstream ss(config_string);

  while (!ss.eof()) {
    std::string line;
    getline(ss, line);
    if (line.empty()) {
      continue;
    }

    command_executor_.Execute(line);
  }
}

void Shell::IgnoreSignalInterrupt() {
  struct sigaction new_action {};
  new_action.sa_handler = SIG_IGN;
  int result = sigaction(SIGINT, &new_action, nullptr);
  if (result) {
    utils::PrintSystemError(std::cerr);
  }
}
