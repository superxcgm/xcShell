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

    command_executor.Execute(line);
  }
}

int Shell::Exit() {
  // todo: do necessary clean up
  return 0;
}

std::string Shell::generatePrompt() {
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
