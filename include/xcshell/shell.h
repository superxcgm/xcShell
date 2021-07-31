#ifndef INCLUDE_XCSHELL_SHELL_H_
#define INCLUDE_XCSHELL_SHELL_H_

#include <string>
#include <vector>
#include <optional>

#include "xcshell/command_executor.h"

class Shell {
 public:
  void Init();
  void Process();
  int Exit();

 private:
  static std::string GeneratePrompt();
  void ExecuteConfig(const std::string &config_string);
  CommandExecutor command_executor_;
};

#endif  // INCLUDE_XCSHELL_SHELL_H_
