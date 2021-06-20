#include "../include/xc_shell.h"

#include <iostream>

#include "../include/command_executor.h"
#include "../include/xc_utils.h"

void XcShell::init() {
  // todo: read ~/.xcshellrc config file
}

void XcShell::process(std::istream &is, std::ostream &os,
                      std::ostream &err_os) {
  while (!is.eof()) {
    os << "> ";
    std::string line;
    getline(is, line);
    if (line.empty()) {
      continue;
    }
    auto [command, args] = parseUserInput(line);

    CommandExecutor::Execute(command, args, os, err_os);
  }
}

int XcShell::exit() {
  // todo: do necessary clean up
  return 0;
}

std::tuple<std::string, std::vector<std::string>> XcShell::parseUserInput(
    const std::string &str) {
  auto parts = xc_utils::split(str);
  std::string command = parts[0];
  parts.erase(parts.begin());
  return {command, parts};
}
