#ifndef INCLUDE_XCSHELL_BUILD_IN_ALIAS_H_
#define INCLUDE_XCSHELL_BUILD_IN_ALIAS_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "./build_in_command.h"

class Alias : public BuildInCommand {
 public:
  int Execute(const std::vector<std::string> &args, std::ostream &os,
              std::ostream &os_err);

  std::string Replace(const std::string& cmd);

  std::string GetName() { return "alias"; }

  ~Alias();

 private:
  int PrintSingleAlias(const std::string &name, std::ostream &os);

  int PrintAllAlias(std::ostream &os);

  void AppendString(std::string &str, const std::string &added_str); //NOLINT

  std::map<std::string, std::string> alias_;
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_ALIAS_H_
