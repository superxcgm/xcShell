#ifndef INCLUDE_XCSHELL_SHELL_H_
#define INCLUDE_XCSHELL_SHELL_H_

#include <string>
#include <vector>

class Shell {
 public:
  void Init();
  void Process();
  int Exit();

 private:
  static std::string generatePrompt();
};

#endif  // INCLUDE_XCSHELL_SHELL_H_
