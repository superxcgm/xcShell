#include "xcshell/build_in/j.h"

#include <fcntl.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "xcshell/constants.h"
#include "xcshell/error_handling.h"
#include "xcshell/utils.h"

int J::Execute(const std::vector<std::string>& args, std::ostream& os,
               std::ostream& os_err) {
  static std::string pre;
  if (args.size() > 1) {
    os_err << "invalid args : Command j and cd have similar functions "
              "and support directory fuzzy matching"
           << std::endl;
    return ERROR_CODE_DEFAULT;
  }

  std::string path = args.empty() ? "~" : args[0];
  if (path == "~") {
    path = utils::GetHomeDir();
  } else if (path == "-") {
    path = pre;
    // cover when pre is empty
    if (path.empty()) {
      path = ".";
    }
  }
  path = utils::GetAbsolutePath(path);
  path = GetFuzzyMatchingDirectory(path);
  pre = utils::GetCurrentWorkingDirectory(os_err);
  ErrorHandling::ErrorDispatchHandler(chdir(path.c_str()),
                                      ErrorHandling::ErrorType::NORMAL_ERROR);
  return SUCCESS;
}

void J::StorageCdHistory(const std::string& path) {
  ReadCdHistory();
  directory_and_weights_map_[path]++;
  int fd = ErrorHandling::ErrorDispatchHandler(
      open(utils::GetAbsolutePath(CD_HISTORY).c_str(), O_WRONLY, 0660),
      ErrorHandling::ErrorType::FATAL_ERROR);
  lock.l_type = F_SETLKW;
  fcntl(fd, F_SETLKW, &lock);
  UpdateCdHistory();
  lock.l_type = F_UNLCK;
  fcntl(fd, F_SETLKW, &lock);
  close(fd);
}

void J::ReadCdHistory() {
  std::string buf;
  std::ifstream in(utils::GetAbsolutePath(CD_HISTORY).c_str(), std::ios::in);
  int line = 0;
  while (getline(in, buf)) {
    std::vector<std::string> directory_and_weights = utils::Split(buf, " ");
    directory_and_weights_map_.insert(std::make_pair(
        directory_and_weights[0], std::stoi(directory_and_weights[1])));
    line++;
  }
}

void J::UpdateCdHistory() {
  std::ofstream file_empty(utils::GetAbsolutePath(CD_HISTORY).c_str(),
                           std::ios_base::out);
  std::ofstream update_file(utils::GetAbsolutePath(CD_HISTORY).c_str(),
                            std::ios::out);
  for (const auto& [k, v] : directory_and_weights_map_) {
    update_file << k << " " << v << std::endl;
  }
  update_file.close();
}

std::string J::GetFuzzyMatchingDirectory(std::string path) {
  for (const auto& [k, v] : directory_and_weights_map_) {
    std::string last_directory = utils::GetLastDir(k);
    if (last_directory.find(path) != std::string::npos) {
      path = k;
      break;
    }
  }
  return path;
}

J::J() { ReadCdHistory(); }
