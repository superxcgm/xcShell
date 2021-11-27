#ifndef INCLUDE_XCSHELL_SHELL_H_
#define INCLUDE_XCSHELL_SHELL_H_

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "xcshell/command_executor.h"

class Shell {
 public:
  Shell(bool load_config, std::string_view cd_history)
      : load_config_(load_config),
        cd_history_(cd_history),
        command_executor_(cd_history_) {}

  void Init();

  void Process();

  int Exit();

 private:
  std::string GeneratePrompt();
  void ExecuteConfig(const std::string& config_string);
  static void IgnoreSignalInterrupt();
  void InitLog();
  static void CreateCdHistory(const std::string& path);

  std::string cd_history_;

  bool load_config_;

  CommandExecutor command_executor_;
};

#endif  // INCLUDE_XCSHELL_SHELL_H_
