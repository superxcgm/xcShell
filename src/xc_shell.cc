#include "../include/xc_shell.h"

#include <iostream>

#include "../include/command_executor.h"
#include "../include/xc_utils.h"

void XcShell::Init() {
  // todo: read ~/.xcshellrc config file
}

void XcShell::Process(std::istream &is, std::ostream &os,
                      std::ostream &err_os) {
  while (!is.eof()) {
    os << "> ";
    std::string line;
    getline(is, line);
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
