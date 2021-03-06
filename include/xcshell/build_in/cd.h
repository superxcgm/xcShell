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
  explicit Cd(J &j) : j_(j) {}
  int Execute(const std::vector<std::string> &args, std::ostream &os,
              std::ostream &os_err) override;
  std::string GetName() override { return "cd"; }
  ~Cd() override = default;
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_CD_H_
