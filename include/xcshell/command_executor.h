#ifndef INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
#define INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_

#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "build_in/build_in.h"
#include "xcshell/CommandParseResult.h"
#include "xcshell/parser.h"

class CommandExecutor {
 public:
  CommandExecutor() : parser_(build_in_) {}

  int Execute(const std::string &line);

  static std::vector<char *> BuildArgv(const std::string &command,
                                       const std::vector<std::string> &args);

 private:
  static int ProcessChild(const CommandParseResult &command_parse_result,
                          bool is_need_pipe, int *pipe_fds, int cmd_numebr);
  static void WaitChildExit(pid_t pid);
  static void output_redirect(const CommandParseResult &command_parse_result);
  static void input_redirect(const CommandParseResult &command_parse_result);
  static void pipe_redirect(int *pipe_fds, int cmd_number);

  BuildIn build_in_;

  Parser parser_;
  static void ResetSignalHandlerForInterrupt();
};

#endif  // INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
