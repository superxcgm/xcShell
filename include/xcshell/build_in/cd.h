#ifndef INCLUDE_XCSHELL_BUILD_IN_CD_H_
#define INCLUDE_XCSHELL_BUILD_IN_CD_H_

#include <vector>
#include <string>

#include "./build_in_command.h"

class Cd : public BuildInCommand {
 public:
  int Execute(const std::vector<std::string>& args);
  std::string GetName() { return "cd"; }
  ~Cd() {}
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_CD_H_
