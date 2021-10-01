#include "xcshell/utils.h"

#include <spdlog/spdlog.h>

#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

std::string utils::ExpandPath(const std::string& path) {
  if (path[0] == '~') {
    return GetHomeDir() + path.substr(1);
  }
  return path;
}

std::string utils::ReadFileText(const std::string& file_name) {
  std::ifstream ifs(file_name);
  if (ifs) {
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    return content;
  } else {
    return "";
  }
}

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
  while (i < str.length() && str[i] == ' ') i++;
  return i;
}

std::string ExtractQuoteString(int start, char quotation_mark,
                               const std::string& str) {
  int i;
  for (i = start; i < str.length(); i++) {
    if (str[i] == quotation_mark) {
      break;
    }
  }
  return str.substr(start, i - start);
}

std::string ExtractStringWithoutQuote(int start, const std::string& str) {
  int i = start;
  for (; i < str.length(); i++) {
    if (str[i] == ' ') {
      break;
    }
    if (str[i - 1] == '=' && (str[i] == '\'' || str[i] == '"')) {
      return str.substr(start, i - start + 1) +
             ExtractQuoteString(i + 1, str[i], str) + str[i];
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
  std::vector<char> quotation_marks = {'\'', '"'};
  for (; i < str.length();) {
    std::string fragment;
    if (str[i] == quotation_marks[0] || str[i] == quotation_marks[1]) {
      fragment = std::move(ExtractQuoteString(i + 1, str[i], str));
      i += 2;  // ignore quotation mark
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

std::string utils::RemoveQuote(const std::string& str) {
  if (str[0] == '\'' && str[str.length() - 1] == '\'') {
    return str.substr(1, str.length() - 2);
  }
  return str;
}

// Todo:
//  1. Split should not trim, it's not Split's responsibility.
//  2. Merge SplitWithSymbol and Split
std::vector<std::string> utils::SpiltWithSymbol(const std::string& str,
                                                const std::string& symbol) {
  std::vector<std::string> str_list;
  size_t left = 0;
  size_t idx;
  for (idx = str.find(symbol); idx != std::string::npos;
       idx = str.find(symbol, idx + 1)) {
    str_list.push_back(Trim(str.substr(left, idx - left)));
    left = idx + 1;
  }

  if (left < str.size() - 1) {
    str_list.push_back(Trim(str.substr(left, str.size() - left)));
  }
  return str_list;
}

std::string utils::GetRandomString(int len) {
  static std::string charset =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  std::string rand_string;
  rand_string.resize(len);
//  Todo: Replace c style random with C++ 11 style
  for (int i = 0; i < len; i++) {
    rand_string[i] = charset[rand() % charset.length()];
  }
  return rand_string;
}

std::string utils::GetCommandExecuteResult(CommandExecutor* commandExecutor,
                                           const std::string& command) {
  // Todo: these redirect should replace with command redirection
  //  once Execute can redirect stderr
  int save_fd_err = dup(STDERR_FILENO);
  std::string temp_file_stdout = GenerateTmpFileName();
  std::string temp_file_stderr = GenerateTmpFileName();
  int fd_out_error =
      open(temp_file_stderr.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0664);
  dup2(fd_out_error, STDERR_FILENO);
  close(fd_out_error);
  commandExecutor->Execute(command + " > " + temp_file_stdout);
  std::string branch_name = ReadFileText(temp_file_stdout);
  // remove \n
  branch_name = branch_name.substr(0, branch_name.size() - 1);
  dup2(save_fd_err, STDERR_FILENO);
  close(save_fd_err);
  remove(temp_file_stdout.c_str());
  remove(temp_file_stderr.c_str());
  return branch_name;
}

std::string utils::GetBranchName(CommandExecutor* commandExecutor) {
  // Todo: Use more fast approach (Read .git/HEAD directly)
  //  instead of use Execute
  auto branch_name = utils::GetCommandExecuteResult(
      commandExecutor, "git branch --show-current");
  spdlog::debug("current branch_name: {}", branch_name);
  std::string red_font_attributes = "\033[31m";
  std::string close_all_attributes = "\033[0m";
  if (!branch_name.empty()) {
    return red_font_attributes + " git(" + branch_name + ")" +
           close_all_attributes;
  }
  return "";
}

std::string utils::Trim(const std::string& str) {
  return RightTrim(LeftTrim(str));
}
std::string utils::LeftTrim(const std::string& str) {
  int i = 0;
  while (i < str.size() && str[i] == ' ') {
    i++;
  }
  return str.substr(i);
}

std::string utils::RightTrim(const std::string& str) {
  int i = str.size() - 1;
  while (i >= 0 && str[i] == ' ') {
    i--;
  }
  return str.substr(0, i + 1);
}
std::string utils::GenerateTmpFileName() {
  return "/tmp/xcShell_tmp_" + GetRandomString(10);
}
