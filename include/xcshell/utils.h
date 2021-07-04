#ifndef INCLUDE_XCSHELL_UTILS_H_
#define INCLUDE_XCSHELL_UTILS_H_

#include <string>
#include <vector>

namespace utils {
  std::vector<std::string> Split(const std::string &str);
  std::vector<std::string> SplitArgs(const std::string &str);
  std::string GetCurrentWorkingDirectory(std::ostream &os_err);
  std::string GetLastDir(const std::string& path);
  std::string GetHomeDir();
  void PrintSystemError(std::ostream &os_err);
}

#endif  // INCLUDE_XCSHELL_UTILS_H_
