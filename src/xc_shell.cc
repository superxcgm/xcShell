#include "../include/xc_shell.h"
#include "../include/xc_utils.h"

#include <iostream>

void XcShell::init() {
  // todo: read ~/.xcshellrc config file
}

void XcShell::process(std::istream &is, std::ostream &os) {
  while (!is.eof()) {
    os << "> ";
    std::string line;
    getline(is, line);
    if (line.empty()) {
      continue;
    }
    auto [command, args] = parseUserInput(line);
    os << command << " ";
    for (const auto &item : args) {
      os << item;
    }
    os << std::endl;
  }
}

int XcShell::exit() {
  // todo: do necessary clean up
  return 0;
}

std::tuple<std::string , std::vector<std::string>>
XcShell::parseUserInput(const std::string &str) {
  auto parts = xc_utils::split(str);
  std::string command = parts[0];
  parts.erase(parts.begin());
  return {command, parts};
}
