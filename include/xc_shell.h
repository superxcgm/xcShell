#ifndef INCLUDE_XC_SHELL_H_
#define INCLUDE_XC_SHELL_H_

#include <iostream>

class XcShell {
 public:
  void init();
  void process(std::istream &is, std::ostream &os);
  int exit();
};

#endif  // INCLUDE_XC_SHELL_H_
