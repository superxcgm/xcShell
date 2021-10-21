#include "xcshell/utils.h"

#include <pwd.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <random>
#include <string>
#include <unordered_map>

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
std::string utils::GetCurrentWorkingDirectory(std::ostream& os_err) {
  char buf[BUFSIZ];
  if (getcwd(buf, BUFSIZ) == nullptr) {
    ErrorHandling::PrintSystemError(os_err);
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
  static std::uniform_int_distribution<unsigned> u(0, charset.size() - 1);
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
  int i = str.size() - 1;
  while (i >= 0 && str[i] == ' ') {
    i--;
  }
  return str.substr(0, i + 1);
}
std::string utils::GenerateTmpFileName() {
  return "/tmp/xcShell_tmp_" + GetRandomString(10);
}

void utils::StorageCatalogHistoryInFile(const std::string& pwd) {
  std::string xcShell_storage_catalog_file =
      "/tmp/xcShell_storage_catalog_file";
  auto catalog_and_weights_map =
      utils::ReadFileWithMap(xcShell_storage_catalog_file, pwd);
  utils::InsertCurrentCatalogInMap(pwd, &catalog_and_weights_map);
  utils::UpdateCatalogFileByMap(xcShell_storage_catalog_file,
                                catalog_and_weights_map);
}

void utils::InsertCurrentCatalogInMap(
    const std::string& pwd,
    std::unordered_map<std::string, int>* catalog_and_weights_map) {
  auto item = catalog_and_weights_map->begin();
  for (; item != catalog_and_weights_map->end(); item++) {
    if (item->first == pwd) {
      item->second++;
    }
  }
  if (catalog_and_weights_map->count(pwd) == 0) {
    catalog_and_weights_map->insert(std::make_pair(pwd, 1));
  }
}

std::unordered_map<std::string, int> utils::ReadFileWithMap(
    const std::string& xcShell_storage_catalog_file, const std::string& pwd) {
  char buf[BUFSIZ];
  std::unordered_map<std::string, int> catalog_and_weights_map;
  std::ifstream in(xcShell_storage_catalog_file);
  int line = 0;
  while (in.getline(buf, BUFSIZ)) {
    std::vector<std::string> contents_and_weights = utils::Split(buf, " ");
    catalog_and_weights_map.insert(std::make_pair(
        contents_and_weights[0], atoi(contents_and_weights[1].c_str())));
    line++;
  }
  return catalog_and_weights_map;
}

void utils::UpdateCatalogFileByMap(
    const std::string& xcShell_storage_catalog_file,
    const std::unordered_map<std::string, int>& catalog_and_weights_map) {
  std::ofstream file_writer(xcShell_storage_catalog_file, std::ios_base::out);
  std::ofstream os;
  auto item = catalog_and_weights_map.begin();
  os.open(xcShell_storage_catalog_file, std::ios::app);
  for (; item != catalog_and_weights_map.end(); item++) {
    os << item->first + " " << item->second << std::endl;
  }
  os.close();
}