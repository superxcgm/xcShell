#ifndef INCLUDE_XCSHELL_BUILD_IN_CD_H_
#define INCLUDE_XCSHELL_BUILD_IN_CD_H_

#include <string>
#include <vector>

#include "./build_in_command.h"
#include "./j.h"

class Cd : public BuildInCommand {
 private:
  J &j_;

 public:
  Cd(J &j) : j_(j) {}
  int Execute(const std::vector<std::string> &args, std::ostream &os,
              std::ostream &os_err);
  std::string GetName() { return "cd"; }
  ~Cd() {}
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_CD_H_
