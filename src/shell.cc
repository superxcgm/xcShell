#include <editline/readline.h>

#include "../include/xcshell/command_executor.h"
#include "../include/xcshell/shell.h"
#include "../include/xcshell/utils.h"

void Shell::Init() {
  // todo: read ~/.xcshellrc config file
}

void Shell::Process() {
  CommandExecutor command_executor;
  while (true) {
    char *line_ptr;
    line_ptr = readline(generatePrompt().c_str());
    if (line_ptr == nullptr) {
      break;
    }
    add_history(line_ptr);
    std::string line = line_ptr;
    free(line_ptr);
    if (line.empty()) {
      continue;
    }
    auto [command, args] = ParseUserInput(line);

    command_executor.Execute(command, args);
  }
}

int Shell::Exit() {
  // todo: do necessary clean up
  return 0;
}

std::tuple<std::string, std::vector<std::string>> Shell::ParseUserInput(
    const std::string &str) {
  auto parts = utils::Split(str);
  std::string command = parts[0];
  parts.erase(parts.begin());
  return {command, parts};
}
std::string Shell::generatePrompt() {
  auto pwd = utils::GetCurrentWorkingDirectory();
  auto home = utils::GetHomeDir();
  std::string dir;
  if (home == pwd) {
    dir = "~";
  } else {
    dir = utils::GetLastDir(pwd);
  }
  return dir + " > ";
}
