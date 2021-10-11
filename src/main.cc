
#include "xcshell/shell.h"

int main(int argc, char **argv) {
  Shell shell;

  shell.Init(argc, argv);

  shell.Process();

  return shell.Exit();
}
