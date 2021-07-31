#include "xcshell/shell.h"
#include <editline/readline.h>

#include <sstream>

#include "xcshell/utils.h"
#include "xcshell/constants.h"

void Shell::Init() {
  auto global_config = utils::ReadFileText(GLOBAL_CONFIG_FILE);
  ExecuteConfig(global_config);

  auto user_config = utils::ReadFileText(utils::ExpandPath(USER_CONFIG_FILE));
  ExecuteConfig(user_config);
}

void Shell::Process() {
  while (true) {
    char *line_ptr;
    line_ptr = readline(GeneratePrompt().c_str());
    if (line_ptr == nullptr) {
      break;
    }
    add_history(line_ptr);
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
  return dir + " > ";
}
void Shell::ExecuteConfig(const std::string &config_string) {
  if (config_string.empty()) {
    return;
  }
  std::istringstream ss(config_string);

  while (!ss.eof()) {
    std::string line;
    getline(ss,line);
    if (line.empty()) {
      continue;
    }

    command_executor_.Execute(line);
  }
}
