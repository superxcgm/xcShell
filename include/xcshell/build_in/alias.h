#ifndef INCLUDE_XCSHELL_BUILD_IN_ALIAS_H_
#define INCLUDE_XCSHELL_BUILD_IN_ALIAS_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <functional>

#include "./build_in_command.h"

class Alias : public BuildInCommand {
 public:
  int Execute(const std::vector<std::string> &args, std::ostream &os,
              std::ostream &os_err) override;

  std::string Replace(const std::string &cmd);

  std::string GetName() override { return "alias"; }

  ~Alias() override = default;

 private:
  int PrintSingleAlias(const std::string &name, std::ostream &os);

  int PrintAllAlias(std::ostream &os);

  void AppendString(std::string &str, const std::string &added_str);  // NOLINT

  // todo: do we need sort?
  std::map<std::string, std::string, std::less<>> alias_;
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_ALIAS_H_
