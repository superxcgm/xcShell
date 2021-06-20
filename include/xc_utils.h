#ifndef INCLUDE_XC_UTILS_H_
#define INCLUDE_XC_UTILS_H_

#include <string>
#include <vector>

#define PrintSystemError(err_os) (err_os << strerror(errno) << std::endl)

namespace xc_utils {
  std::vector<std::string> Split(const std::string &str);
  std::string GetCurrentWorkingDirectory(std::ostream &os_err);
  std::string GetLastDir(const std::string& path);
  std::string GetHomeDir();
}

#endif  // INCLUDE_XC_UTILS_H_
