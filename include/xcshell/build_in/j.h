#ifndef XCSHELL_J_H
#define XCSHELL_J_H

#include <string>
#include <vector>

#include "./build_in_command.h"

class J : public BuildInCommand {
 public:
  int Execute(const std::vector<std::string> &args, std::ostream &os,
              std::ostream &os_err);
  std::string GetName() { return "j"; }
  ~J() {}
};
#endif  // XCSHELL_J_H
