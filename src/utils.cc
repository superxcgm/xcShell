#include <pwd.h>
#include <unistd.h>

#include <sstream>
#include <iostream>
#include <string>

#include "../include/xcshell/utils.h"

std::vector<std::string> utils::Split(const std::string& str) {
  std::stringstream ss(str);
  std::vector<std::string> parts;

  while (ss) {
    std::string part;
    ss >> part;
    parts.push_back(part);
  }

  if (parts[parts.size() - 1] == "") {
    parts.pop_back();
  }

  return parts;
}
std::string utils::GetCurrentWorkingDirectory() {
  char buf[BUFSIZ];
  if (getcwd(buf, BUFSIZ) == nullptr) {
    utils::PrintSystemError();
    return "";
  }
  return buf;
}
std::string utils::GetLastDir(const std::string& path) {
  if (path == "/") {
    return "/";
  }
  std::string path_std =
      path[path.size() - 1] == '/' ? path.substr(0, path.size() - 1) : path;
  auto idx = path_std.rfind('/');
  return path_std.substr(idx + 1);
}
std::string utils::GetHomeDir() {
  static std::string home_dir;
  if (!home_dir.empty()) {
    return home_dir;
  }

  // todo: replace with getpwuid_r
  auto pw = getpwuid(getuid()); //NOLINT
  home_dir = pw->pw_dir;
  return home_dir;
}
void utils::PrintSystemError() {
  std::cerr << strerror(errno) << std::endl;
}
