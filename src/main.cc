
#include "../include/xcshell/shell.h"

int main() {
  Shell shell;

  shell.Init();

  shell.Process();

  return shell.Exit();
}
