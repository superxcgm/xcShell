#ifndef INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
#define INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_

#include <memory>
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
                          bool is_pipe_redirect, int *pipe_fds, int cmd_number);
  static void WaitChildExit(pid_t pid);
  static void OutputRedirect(const CommandParseResult &command_parse_result);
  static void InputRedirect(const CommandParseResult &command_parse_result);
  static void PipeRedirect(int *pipe_fds, int cmd_number);

  BuildIn build_in_;

  Parser parser_;
  static void ResetSignalHandlerForInterrupt();
  static void RedirectSelector(const CommandParseResult &command_parse_result,
                               bool is_pipe_redirect, int *pipe_fds,
                               int cmd_number);
  static void PipeRedirectOut(const int *pipe_fds);
  static void PipeRedirectIn(const int *pipe_fds);
  std::shared_ptr<CommandParseResult> BuildInCommandPipeExecute(
      int save_fd, std::shared_ptr<CommandParseResult> built_In_Command_ptr,
      const int *pipe_fds);
  static void ProcessFather(
      const std::vector<CommandParseResult> &command_parse_result_list,
      const int *pipe_fds, int cmd_number, pid_t pid);
};

#endif  // INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
