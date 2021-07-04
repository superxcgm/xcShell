#include "../include/xcshell/utils.h"

#include <pwd.h>
#include <unistd.h>

#include <sstream>
#include <string>

std::vector<std::string> utils::Split(const std::string& str) {
  std::stringstream ss(str);
  std::vector<std::string> parts;

  while (ss) {
    std::string part;
    ss >> part;
    parts.push_back(part);
  }

  if (parts[parts.size() - 1].empty()) {
    parts.pop_back();
  }

  return parts;
}
std::string utils::GetCurrentWorkingDirectory(std::ostream& os_err) {
  char buf[BUFSIZ];
  if (getcwd(buf, BUFSIZ) == nullptr) {
    utils::PrintSystemError(os_err);
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
  auto pw = getpwuid(getuid());  // NOLINT
  home_dir = pw->pw_dir;
  return home_dir;
}
void utils::PrintSystemError(std::ostream& os_err) {
  os_err << strerror(errno) << std::endl;
}

int NextNonSpacePos(int start, const std::string& str) {
  int i = start;
  while (i < str.length() && str[i] == ' ')
    i++;
  return i;
}

std::string ExtractQuoteString(int start, char quotation_mark, const std::string & str) {
  int i;
  for (i = start; i < str.length(); i++ ){
    if (str[i] == quotation_mark) {
      break;
    }
  }
  return str.substr(start, i - start);
}

std::string ExtractStringWithoutQuote(int start, const std::string &str) {
  int i = start;
  for (; i < str.length(); i++ ){
    if (str[i] == ' ') {
      break;
    }
    if (str[i - 1] == '=' && (str[i] == '\'' || str[i] == '"')) {
      return str.substr(start, i - start + 1) + ExtractQuoteString(i + 1, str[i], str) + str[i];
    }
  }
  return str.substr(start, i - start);
}

std::vector<std::string> utils::SplitArgs(const std::string& str) {
  if (str.empty()) {
    return {};
  }
  std::vector<std::string> parts;
  int i = NextNonSpacePos(0, str);
  std::vector<char> quotation_marks = {'\'', '"'} ;
  for (; i < str.length(); ) {
    std::string fragment;
    if (str[i] == quotation_marks[0] || str[i] == quotation_marks[1]) {
      fragment = std::move(ExtractQuoteString(i + 1, str[i], str));
      i += 2; // ignore quotation mark
    } else {
      fragment = std::move(ExtractStringWithoutQuote(i, str));
    }
    parts.push_back(fragment);
    i = NextNonSpacePos(i + fragment.size(), str);
  }
  if (i < str.length()) {
    parts.push_back(str.substr(i));
  }

  return parts;
}
