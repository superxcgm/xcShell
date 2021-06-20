#include <iostream>
#include "../include/xc_shell.h"

int main() {
    XcShell xc_shell;

    xc_shell.Init();

    xc_shell.Process(std::cin, std::cout, std::cerr);

    return xc_shell.Exit();
}
