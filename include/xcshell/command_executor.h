#ifndef INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
#define INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_

#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "build_in/build_in.h"
#include "xcshell/command_parse_result.h"
#include "xcshell/parser.h"

class CommandExecutor {
 public:
  explicit CommandExecutor(const std::string &cd_history)
      : build_in_(cd_history), parser_(build_in_) {}

  int Execute(const std::string &line);

  static std::vector<char *> BuildArgv(const std::string &command,
                                       const std::vector<std::string> &args);

 private:
  static int ProcessChild(const CommandParseResult &command_parse_result,
                          const std::vector<std::array<int, 2>> &pipe_fds_list,
                          int cmd_number, bool is_last_command);
  static void WaitChildExit(pid_t pid);
  static int GetFileDescriptor(const std::string &file_name, bool is_append);
  static void OutputRedirect(const CommandParseResult &command_parse_result);
  static void InputRedirect(const CommandParseResult &command_parse_result);
  static void PipeRedirect(const std::vector<std::array<int, 2>> &pipe_fds_list,
                           int cmd_number, bool is_last_command);

  BuildIn build_in_;

  Parser parser_;
  static void ResetSignalHandlerForInterrupt();
  static void RedirectSelector(
      const CommandParseResult &command_parse_result,
      const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number,
      bool is_last_command);
  static void CloseAllPipeAndWaitChildProcess(
      const std::vector<std::array<int, 2>> &pipe_fds_list,
      const std::vector<pid_t> &child_pids);
  static void PipeRedirectFirst(
      const std::vector<std::array<int, 2>> &pipe_fds_list);
  static void PipeRedirectMiddle(
      const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number);
  static void PipeRedirectEnd(
      const std::vector<std::array<int, 2>> &pipe_fds_list, int cmd_number);
  void BuildInCommandExecute(
      int save_fd, CommandParseResult *built_In_Command_ptr,
      const std::vector<std::array<int, 2>> &pipe_fds_list);
  static std::vector<std::array<int, 2>> CreatePipe(
      const std::vector<CommandParseResult> &command_parse_result_list);
  void LogCommandParseResultList(
      const std::vector<CommandParseResult> &command_parse_result_list);
  static void ErrorOutputRedirect(
      const CommandParseResult &command_parse_result);
};

#endif  // INCLUDE_XCSHELL_COMMAND_EXECUTOR_H_
