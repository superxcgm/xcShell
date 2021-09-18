#ifndef INCLUDE_XCSHELL_PARSER_H_
#define INCLUDE_XCSHELL_PARSER_H_

#include <string>
#include <utility>
#include <vector>

#include "build_in/build_in.h"
#include "xcshell/CommandParseResult.h"

class Parser {
 public:
  Parser(BuildIn &build_in): build_in_(build_in) { }
  CommandParseResult ParseUserInputLine(const std::string &input_line);

 private:
  BuildIn &build_in_;
  std::tuple<std::string, std::vector<std::string>> getCommandAndSuffix(const std::string &input_line);
};
#endif  // INCLUDE_XCSHELL_PARSER_H_
