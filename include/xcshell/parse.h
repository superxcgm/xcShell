//
// Created by lv qiang on 2021/9/15.
//

#ifndef XCSHELL_PARSE_H
#define XCSHELL_PARSE_H

#include <string>
#include <utility>
#include <vector>

#include "build_in/build_in.h"
#include "xcshell/parseTypeElement.h"

class Parse {
 public:
  Parse() = default;
  ParseTypeElement parseUserInputLine(const std::string &input_line);

 private:
  BuildIn build_in_;
};

#endif  // XCSHELL_PARSE_H
