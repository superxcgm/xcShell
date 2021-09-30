#include "xcshell/utils.h"

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

char* LeftTrim(char* str) {
  if (str == nullptr || *str == '\0') {
    return str;
  }
  int len = 0;
  char* p = str;
  while (*p != '\0' && isspace(*p)) {
    ++p;
    ++len;
  }
  memmove(str, p, strlen(str) - len + 1);
  return str;
}

char* RightTrim(char* str) {
  if (str == nullptr || *str == '\0') {
    return str;
  }
  auto len = strlen(str);
  char* p = str + len - 1;
  while (p >= str && isspace(*p)) {
    *p = '\0';
    --p;
  }
  return str;
}

char* Trim(char* str) {
  str = RightTrim(str);
  str = LeftTrim(str);
  return str;
}

std::vector<std::string> utils::SpiltWithSymbol(const std::string& str,
                                                const std::string& symbol) {
  std::vector<std::string> str_list;
  char* save_ptr = nullptr;
  char* p = const_cast<char*>(str.c_str());
  char* input = strdup(p);
  while ((input = strtok_r(input, symbol.c_str(), &save_ptr)) != nullptr) {
    input = Trim(input);
    str_list.emplace_back(input);
    input = nullptr;
  }
  free(input);
  return str_list;
}

std::string utils::GetRandomString(int len) {
  static std::string charset =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  std::string rand_string;
  rand_string.resize(len);
  srand(time(nullptr));
  for (int i = 0; i < len; i++) {
    rand_string[i] = charset[rand() % charset.length()];
  }
  return rand_string;
}

std::string utils::GetCommandExecuteResult(CommandExecutor* commandExecutor,
                                           const std::string& command) {
  std::string result;
  int save_fd_out = dup(STDOUT_FILENO);
  int save_fd_err = dup(STDERR_FILENO);
  std::string temporary_file_correct =
      "xcShell_temp_correct_" + utils::GetRandomString(10) + ".txt";
  std::string temporary_file_error =
      "xcShell_temp_error_" + utils::GetRandomString(10) + ".txt";
  int fd_out_correct =
      open(temporary_file_correct.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0664);
  int fd_out_error =
      open(temporary_file_error.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0664);
  dup2(fd_out_correct, STDOUT_FILENO);
  close(fd_out_correct);
  dup2(fd_out_error, STDERR_FILENO);
  close(fd_out_error);
  commandExecutor->Execute(command);
  std::ifstream fin(temporary_file_correct);
  while (fin >> result) {
  }
  dup2(save_fd_out, STDOUT_FILENO);
  dup2(save_fd_err, STDERR_FILENO);
  remove(temporary_file_correct.c_str());
  remove(temporary_file_error.c_str());
  return result;
}

std::string utils::GetBranchName(CommandExecutor* commandExecutor) {
  auto branch_name = utils::GetCommandExecuteResult(
      commandExecutor, R"(git branch | grep "^\*" | sed 's/^..//')");
  std::string red_font_attributes = "\033[31m";
  std::string close_all_attributes = "\033[0m";
  if (!branch_name.empty()) {
    return red_font_attributes + " git(" + branch_name + ")" +
           close_all_attributes;
  }
  return "";
}
