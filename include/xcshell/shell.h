#ifndef INCLUDE_XCSHELL_SHELL_H_
#define INCLUDE_XCSHELL_SHELL_H_

#include <string>
#include <tuple>
#include <vector>

class Shell {
 public:
  void Init();
  void Process();
  int Exit();

 private:
  // todo: remove basic type below using typedef or something else
  static std::tuple<std::string, std::vector<std::string>> ParseUserInput(
      const std::string &str);

  static std::string generatePrompt();
};

#endif  // INCLUDE_XCSHELL_SHELL_H_
