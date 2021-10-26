#ifndef INCLUDE_XCSHELL_BUILD_IN_J_H_
#define INCLUDE_XCSHELL_BUILD_IN_J_H_

#include <fcntl.h>

#include <string>
#include <utility>
#include <vector>

#include "./build_in_command.h"
#include "unordered_map"
#include "xcshell/utils.h"

class J : public BuildInCommand {
 private:
  struct flock lock {};
  std::unordered_map<std::string, int> directory_and_weights_map_;
  void ReadCdHistory();
  void UpdateCdHistory();
  std::string GetFuzzyMatchingDirectory(std::string path);

 public:
  J();
  int Execute(const std::vector<std::string>& args, std::ostream& os,
              std::ostream& os_err);
  std::string GetName() { return "j"; }
  void StorageCdHistory(const std::string& path);
  ~J() {}
};
#endif  // INCLUDE_XCSHELL_BUILD_IN_J_H_
