#ifndef INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
#define INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_

#include <string>
#include <tuple>
#include <vector>

#include "build_in/build_in.h"
#include "xcshell/parser.h"

class CommandExecutor {
 public:
  int Execute(const std::string &line);

 private:
  static int ProcessChild(const CommandParseResult &commandParseResult);
  static void WaitChildExit(pid_t pid);

  BuildIn build_in_;

  Parser parser_;
};

#endif  // INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
