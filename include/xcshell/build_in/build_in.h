#ifndef INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_H_
#define INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "./build_in_command.h"
#include "./alias.h"

class BuildIn{
 public:
  BuildIn();

  bool Exist(const std::string &cmd);

  int Execute(const std::string& cmd, const std::vector<std::string> &args);

  std::shared_ptr<Alias> GetAlias();

 private:
  std::map<std::string , std::shared_ptr<BuildInCommand>> commands_;
  std::shared_ptr<Alias> alias_;
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_H_
