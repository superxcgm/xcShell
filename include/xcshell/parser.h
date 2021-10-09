#ifndef INCLUDE_XCSHELL_PARSER_H_
#define INCLUDE_XCSHELL_PARSER_H_

#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "build_in/build_in.h"
#include "xcshell/command_parse_result.h"

class Parser {
 public:
  explicit Parser(BuildIn &build_in) : build_in_(build_in) {}
  std::vector<CommandParseResult> ParseUserInputLine(
      const std::string &input_line);

 private:
  BuildIn &build_in_;
  std::tuple<std::string, std::vector<std::string>> GetCommandAndSuffix(
      const std::string &input_line);
  static CommandParseResult BuildParseResultWithRedirect(
      const std::vector<std::string> &command_with_args,
      const std::string &command);

  static bool IsInputRedirectSymbol(const std::string &command_with_arg);
  static bool IsOutputRedirectSymbol(const std::string &command_with_arg);
  static bool IsRedirectOverwrite(const std::string &command_with_arg);
  static bool IsErrorRedirect(const std::string &command_with_arg);
  static bool IsRedirect(const std::string &command_with_arg);
};
#endif  // INCLUDE_XCSHELL_PARSER_H_
