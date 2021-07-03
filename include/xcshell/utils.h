#ifndef INCLUDE_XCSHELL_UTILS_H_
#define INCLUDE_XCSHELL_UTILS_H_

#include <string>
#include <vector>

namespace utils {
  std::vector<std::string> Split(const std::string &str);
  std::string GetCurrentWorkingDirectory();
  std::string GetLastDir(const std::string& path);
  std::string GetHomeDir();
  void PrintSystemError();
}

#endif  // INCLUDE_XCSHELL_UTILS_H_
