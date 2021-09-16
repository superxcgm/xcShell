#ifndef XCSHELL_REDIRECTELEMENT_H
#define XCSHELL_REDIRECTELEMENT_H

#include <string>
#include <utility>
#include <vector>

struct RedirectElement {
  std::string command;
  std::vector<std::string> args;
  std::string input_redirect_file;
  std::string output_redirect_file;
  enum output_mode { overwrite, append } output_mode;
  enum cmd_type { normal, direct } cmd_type;
};
#endif
