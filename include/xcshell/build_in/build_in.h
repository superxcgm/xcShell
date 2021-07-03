#ifndef INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_H_
#define INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_H_

#include <map>
#include <string>
#include <vector>
#include <memory>

#include "./build_in_command.h"

class BuildIn{
 public:
  BuildIn();

  bool Exist(const std::string &cmd);

  int Execute(const std::string& cmd, const std::vector<std::string> &args);

 private:
  std::map<std::string , std::shared_ptr<BuildInCommand>> commands_;
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_H_
