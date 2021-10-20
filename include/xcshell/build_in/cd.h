#ifndef INCLUDE_XCSHELL_BUILD_IN_CD_H_
#define INCLUDE_XCSHELL_BUILD_IN_CD_H_

#include <string>
#include <vector>

#include "./build_in_command.h"
#include "xcshell/error_handling.h"

class Cd : public BuildInCommand {
 public:
  int Execute(const std::vector<std::string> &args, std::ostream &os,
              std::ostream &os_err);
  std::string GetName() { return "cd"; }
  ~Cd() {}
  ErrorHandling error_handling_;
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_CD_H_
