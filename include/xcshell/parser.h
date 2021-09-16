#ifndef XCSHELL_PARSER_H
#define XCSHELL_PARSER_H

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

#endif  // XCSHELL_PARSER_H
