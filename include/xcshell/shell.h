#ifndef INCLUDE_XCSHELL_SHELL_H_
#define INCLUDE_XCSHELL_SHELL_H_

#include <optional>
#include <string>
#include <vector>

#include "xcshell/command_executor.h"

class Shell {
 public:
  void Init();

  void Process();

  int Exit();

 private:
  static std::string GeneratePrompt();
  void ExecuteConfig(const std::string &config_string);
  static void IgnoreSignalInterrupt();

  CommandExecutor command_executor_;
  static std::string GetCommandExecuteResult(
      const std::string& command);
};

#endif  // INCLUDE_XCSHELL_SHELL_H_
