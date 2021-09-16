#ifndef INCLUDE_XCSHELL_PARSER_H_
#define INCLUDE_XCSHELL_PARSER_H_

#include <string>
#include <utility>
#include <vector>

#include "build_in/build_in.h"
#include "xcshell/CommandParseResult.h"

class Parser {
 public:
  Parser() = default;
  CommandParseResult ParseUserInputLine(const std::string &input_line);
  static std::vector<char *> BuildArgv(const std::string &command,
                                       const std::vector<std::string> &args);

 private:
  BuildIn build_in_;
};

#endif  // INCLUDE_XCSHELL_PARSER_H_
