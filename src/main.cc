#include <iostream>
#include "../include/xc_shell.h"

int main() {
    XcShell xc_shell;

    xc_shell.init();

    xc_shell.process(std::cin, std::cout, std::cerr);

    return xc_shell.exit();
}
