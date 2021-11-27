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
  std::optional<std::vector<CommandParseResult>> Parse(
      const std::string &input_line);

 private:
  BuildIn &build_in_;
  std::optional<std::tuple<std::string, std::vector<std::string>>> ParseCommand(
      const std::string &input_line);
  static CommandParseResult BuildParseResultWithRedirect(
      const std::vector<std::string> &origin_args, const std::string &command);

  static bool IsInputRedirectSymbol(const std::string &arg);
  static bool IsOutputRedirectSymbol(const std::string &arg);
  static bool IsErrorRedirectSymbol(const std::string &arg);
  static bool IsErrorToStdoutRedirect(const std::string &arg);
  static bool IsRedirect(const std::string &arg);
  static std::pair<std::string, std::string> SplitCommandNameAndArgs(
      const std::string &command);
  static std::optional<std::vector<std::string>> SplitArgs(
      const std::string &str);
  static std::string ReplaceVariable(const std::string &str);
  static std::pair<std::string, int> ExtractVariable(const std::string &str,
                                                     int start);
  static std::optional<std::pair<std::string, int>> ExtractQuoteString(
      int start, char quotation_mark, const std::string &str,
      std::ostream &os_err);
  static std::optional<std::pair<std::string, int>> ExtractStringWithoutQuote(
      int start, const std::string &str);
  static int NextNonSpacePos(int start, const std::string &str);

  static std::string redirect_output_overwrite_;
  static std::string redirect_output_append_;
  static std::string redirect_error_output_overwrite_;
  static std::string redirect_error_output_append_;
  static std::string redirect_error_to_stdout_;
  static std::string redirect_input_;
  static std::string redirect_pipe_;
};
#endif  // INCLUDE_XCSHELL_PARSER_H_
