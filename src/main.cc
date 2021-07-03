#include <iostream>

#include "../include/xcshell/shell.h"

int main() {
  Shell shell;

  shell.Init();

  shell.Process(std::cin, std::cout, std::cerr);

    return shell.Exit();
}
