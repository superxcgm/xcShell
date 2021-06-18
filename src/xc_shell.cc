#include "../include/xc_shell.h"

#include <iostream>

void XcShell::init() {
  // todo: read ~/.xcshellrc config file
}

void XcShell::process() {
  // todo: get user input, execute loop just like REPL
  std::cout << "Hello world" << std::endl;
}

int XcShell::exit() {
  // todo: do necessary clean up
  return 0;
}
