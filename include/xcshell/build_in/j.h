#ifndef INCLUDE_XCSHELL_BUILD_IN_J_H_
#define INCLUDE_XCSHELL_BUILD_IN_J_H_

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
#endif  // INCLUDE_XCSHELL_BUILD_IN_J_H_
