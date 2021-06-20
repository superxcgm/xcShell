#ifndef INCLUDE_XC_SHELL_H_
#define INCLUDE_XC_SHELL_H_

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

class XcShell {
 public:
  void Init();
  void Process(std::istream &is, std::ostream &os, std::ostream &err_os);
  int Exit();

 private:
  // todo: remove basic type below using typedef or something else
  static std::tuple<std::string, std::vector<std::string>> ParseUserInput(
      const std::string &str);

  static std::string generatePrompt(std::ostream &err_os);
};

#endif  // INCLUDE_XC_SHELL_H_
