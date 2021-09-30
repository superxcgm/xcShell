#ifndef INCLUDE_XCSHELL_COMMANDPARSERESULT_H_
#define INCLUDE_XCSHELL_COMMANDPARSERESULT_H_

#include <string>
#include <utility>
#include <vector>

struct command_parse_result {
  std::string command;
  std::vector<std::string> args;
  std::string input_redirect_file;
  std::string output_redirect_file;
  bool output_is_append = false;
};
#endif  // INCLUDE_XCSHELL_COMMANDPARSERESULT_H_
