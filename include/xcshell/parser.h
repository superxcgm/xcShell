#ifndef INCLUDE_XCSHELL_PARSER_H_
#define INCLUDE_XCSHELL_PARSER_H_

#include <string>
#include <utility>
#include <vector>

#include "build_in/build_in.h"
#include "xcshell/redirectelement.h"

class Parser {
 public:
  Parser() = default;
  RedirectElement parseUserInputLine(const std::string &input_line);

 private:
  BuildIn build_in_;
};

#endif  // INCLUDE_XCSHELL_PARSER_H_
