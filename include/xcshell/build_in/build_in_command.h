#ifndef INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_COMMAND_H_
#define INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_COMMAND_H_

#include <string>
#include <vector>

class BuildInCommand {
 public:
  virtual int Execute(const std::vector<std::string>& args) = 0;
  virtual std::string GetName() = 0;
  virtual ~BuildInCommand(){}
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_COMMAND_H_
