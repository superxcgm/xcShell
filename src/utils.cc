#include "xcshell/utils.h"

#include <pwd.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>

#include "xcshell/error_handling.h"

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
  return Split(str, " ");
}
std::string utils::GetCurrentWorkingDirectory() {
  return std::filesystem::current_path().string();
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
  home_dir = getenv("HOME");
  return home_dir;
}

std::string utils::RemoveQuote(const std::string& str) {
  std::vector<char> quotation_marks = {'\'', '"'};
  if ((str[0] == quotation_marks[0] &&
       str[str.length() - 1] == quotation_marks[0]) ||
      (str[0] == quotation_marks[1] &&
       str[str.length() - 1] == quotation_marks[1])) {
    return str.substr(1, str.length() - 2);
  }
  return str;
}

std::vector<std::string> utils::Split(const std::string& str,
                                      const std::string& delim) {
  std::vector<std::string> str_list;
  size_t left = 0;
  size_t idx;
  for (idx = str.find(delim); idx != std::string::npos;
       idx = str.find(delim, idx + 1)) {
    str_list.push_back(str.substr(left, idx - left));
    left = idx + 1;
  }

  if (left < str.size()) {
    str_list.push_back(str.substr(left, str.size() - left));
  }
  return str_list;
}

std::string utils::GetRandomString(int len) {
  static std::string charset =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  static std::default_random_engine e(time(nullptr));
  static std::uniform_int_distribution<decltype(charset.size())> u(
      0, charset.size() - 1);
  std::string rand_string;
  rand_string.resize(len);
  for (int i = 0; i < len; i++) {
    rand_string[i] = charset[u(e)];
  }
  return rand_string;
}

std::string utils::GetCommandExecuteResult(CommandExecutor* commandExecutor,
                                           const std::string& command) {
  std::string temp_file_stdout = GenerateTmpFileName();
  std::string temp_file_stderr = GenerateTmpFileName();
  commandExecutor->Execute(command + " > " + temp_file_stdout + " 2> " +
                           temp_file_stderr);
  std::string branch_name = ReadFileText(temp_file_stdout);
  // remove \n
  branch_name = branch_name.substr(0, branch_name.size() - 1);
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
  int64_t i = str.size() - 1;
  while (i >= 0 && str[i] == ' ') {
    i--;
  }
  return str.substr(0, i + 1);
}
std::string utils::GenerateTmpFileName() {
  return "/tmp/xcShell_tmp_" + GetRandomString(10);
}
std::string utils::GetAbsolutePath(std::string path) {
  if (path.find('~') != std::string::npos) {
    return path.replace(path.find('~'), 1, utils::GetHomeDir());
  }
  return path;
}

std::string utils::GetDirPath(std::string path) {
  const size_t last_slash_idx = path.rfind('/');
  if (std::string::npos != last_slash_idx) {
    return path.substr(0, last_slash_idx);
  }
  return path;
}
