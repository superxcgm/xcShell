#ifndef INCLUDE_XCSHELL_BUILD_IN_ALIAS_H_
#define INCLUDE_XCSHELL_BUILD_IN_ALIAS_H_

#include <iostream>
#include <vector>
#include <string>

#include "./build_in_command.h"

class Alias: public BuildInCommand {
 public:
  int Execute(const std::vector<std::string> &args, std::ostream &os,
              std::ostream &os_err);
  std::string GetName() { return "cd"; }
  ~Alias();
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_ALIAS_H_
