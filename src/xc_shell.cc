#include "../include/xc_shell.h"

#include <iostream>

void XcShell::init() {
  // todo: read ~/.xcshellrc config file
}

void XcShell::process(std::istream &is, std::ostream &os) {
  while (!is.eof()) {
    os << "> ";
    std::string line;
    getline(is, line);
    os << line << std::endl;
  }
}

int XcShell::exit() {
  // todo: do necessary clean up
  return 0;
}
