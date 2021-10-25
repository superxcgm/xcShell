#ifndef INCLUDE_XCSHELL_BUILD_IN_J_H_
#define INCLUDE_XCSHELL_BUILD_IN_J_H_

#include <string>
#include <vector>

#include "./build_in_command.h"
#include "unordered_map"
#include "xcshell/utils.h"

class J : public BuildInCommand {
 private:
  std::vector<std::pair<std::string, int>> directory_and_weights_list;
  std::unordered_map<std::string, int> directory_and_weights_map;
  std::vector<std::pair<std::string, int>> SortWithMapValueByVector();
  void ReadHistoryFile();
  void UpdateDirectoryFileByVector();
  std::string GetFuzzyMatchingDirectory(std::string path);
  const std::string cd_history =
      utils::GetHomeDir().append("/.xcShell/cd_history.txt");
  const std::string cd_history_path =
      utils::GetHomeDir().append("/.xcShell");
 public:
  J();
  int Execute(const std::vector<std::string>& args, std::ostream& os,
              std::ostream& os_err);
  std::string GetName() { return "j"; }
  void StorageDirectoryHistoryInFile(const std::string& path);
  ~J() {}
  void CreateCdHistory() const;
};
#endif  // INCLUDE_XCSHELL_BUILD_IN_J_H_
