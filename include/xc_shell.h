#ifndef INCLUDE_XC_SHELL_H_
#define INCLUDE_XC_SHELL_H_

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

class XcShell {
 public:
  void init();
  void process(std::istream &is, std::ostream &os, std::ostream &err_os);
  int exit();

 private:
  // todo: remove basic type below using typedef or something else
  static std::tuple<std::string , std::vector<std::string>>
  parseUserInput(const std::string& str);
};

#endif  // INCLUDE_XC_SHELL_H_
