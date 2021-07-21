#ifndef INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
#define INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_

#include <string>
#include <vector>
#include <tuple>

#include "build_in/build_in.h"

class CommandExecutor {
 public:
  int Execute(const std::string &line);

 private:
  static std::vector<char *> BuildArgv(const std::string &command,
                                       const std::vector<std::string> &args);
  static int ProcessChild(const std::string &command,
                          const std::vector<std::string> &args);
  static void WaitChildExit(pid_t pid);
  // todo: remove basic type below using typedef or something else
  std::tuple<std::string, std::vector<std::string>> ParseUserInput(
      const std::string &input_line);
  BuildIn build_in_;
};

#endif  // INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
