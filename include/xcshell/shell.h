#ifndef INCLUDE_XCSHELL_SHELL_H_
#define INCLUDE_XCSHELL_SHELL_H_

#include <optional>
#include <string>
#include <vector>

#include "xcshell/command_executor.h"
#include "xcshell/error_handling.h"

class Shell {
 public:
  void Init(int i, char** pString);

  void Process();

  int Exit();

 private:
  std::string GeneratePrompt();
  void ExecuteConfig(const std::string& config_string);
  static void IgnoreSignalInterrupt();
  void InitLog();

  CommandExecutor command_executor_;
  ErrorHandling error_handling_;
};

#endif  // INCLUDE_XCSHELL_SHELL_H_
