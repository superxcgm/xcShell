#include "../include/xc_shell.h"

#include <editline/readline.h>

#include "../include/command_executor.h"
#include "../include/xc_utils.h"

void XcShell::Init() {
  // todo: read ~/.xcshellrc config file
}

void XcShell::Process(std::istream &is, std::ostream &os,
                      std::ostream &err_os) {
  while (true) {
    char *line_ptr;
    line_ptr = readline(generatePrompt(err_os).c_str());
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

    CommandExecutor::Execute(command, args, os, err_os);
  }
}

int XcShell::Exit() {
  // todo: do necessary clean up
  return 0;
}

std::tuple<std::string, std::vector<std::string>> XcShell::ParseUserInput(
    const std::string &str) {
  auto parts = xc_utils::Split(str);
  std::string command = parts[0];
  parts.erase(parts.begin());
  return {command, parts};
}
std::string XcShell::generatePrompt(std::ostream &err_os) {
  auto pwd = xc_utils::GetCurrentWorkingDirectory(err_os);
  auto home = xc_utils::GetHomeDir();
  std::string dir;
  if (home == pwd) {
    dir = "~";
  } else {
    dir = xc_utils::GetLastDir(pwd);
  }
  return dir + " > ";
}
