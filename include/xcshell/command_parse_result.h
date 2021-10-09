#ifndef INCLUDE_XCSHELL_COMMAND_PARSE_RESULT_H_
#define INCLUDE_XCSHELL_COMMAND_PARSE_RESULT_H_

#include <string>
#include <utility>
#include <vector>

struct CommandParseResult {
  std::string command;
  std::vector<std::string> args;
  std::string input_redirect_file;
  std::string output_redirect_file;
  std::string error_redirect_file;
  bool output_is_append;
  bool is_error_redirect;
};

#endif  // INCLUDE_XCSHELL_COMMAND_PARSE_RESULT_H_
