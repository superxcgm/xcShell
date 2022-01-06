#ifndef INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_H_
#define INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_H_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "./alias.h"
#include "./build_in_command.h"

class BuildIn {
 public:
  explicit BuildIn(std::string_view cd_history);

  bool Exist(const std::string &cmd);

  int Execute(const std::string &cmd, const std::vector<std::string> &args);

  std::shared_ptr<Alias> GetAlias() const;

 private:
  // todo: do we need sort?
  std::map<std::string, std::shared_ptr<BuildInCommand>, std::less<>> commands_;
  std::shared_ptr<Alias> alias_{std::make_shared<Alias>()};
};

#endif  // INCLUDE_XCSHELL_BUILD_IN_BUILD_IN_H_
