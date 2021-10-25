#include "xcshell/build_in/j.h"

#include <sys/stat.h>
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
    os_err << "invalid args" << std::endl;
    return ERROR_CODE_DEFAULT;
  }

  std::string path = args.empty() ? "~" : args[0];
  if (path.find('~') != -1) {
    path = path.replace(path.find('~'), 1, utils::GetHomeDir());
  } else if (path == "-") {
    path = pre;
    // cover when pre is empty
    if (path.empty()) {
      path = ".";
    }
  }
  path = GetFuzzyMatchingDirectory(path);
  pre = utils::GetCurrentWorkingDirectory(os_err);
  ErrorHandling::ErrorDispatchHandler(chdir(path.c_str()),
                                      ErrorHandling::ErrorType::NORMAL_ERROR);
  return 0;
}

void J::StorageDirectoryHistoryInFile(const std::string& path) {
  ReadHistoryFile();
  directory_and_weights_map_[path]++;
  UpdateDirectoryFileByVector();
}

void J::ReadHistoryFile() {
  std::string buf;
  std::ifstream in(CD_HISTORY.c_str(), std::ios::in);
  int line = 0;
  while (getline(in, buf)) {
    std::vector<std::string> directory_and_weights = utils::Split(buf, " ");
    directory_and_weights_map_.insert(
        std::make_pair(directory_and_weights[0],
                       utils::Atoi(directory_and_weights[1].c_str())));
    line++;
  }
}

void J::UpdateDirectoryFileByVector() {
  std::ofstream file_empty(CD_HISTORY.c_str(), std::ios_base::out);
  std::ofstream update_file(CD_HISTORY, std::ios::app);
  auto item = directory_and_weights_map_.begin();
  for (; item != directory_and_weights_map_.end(); item++) {
    update_file << item->first + " " << item->second << std::endl;
  }
  update_file.close();
}

std::string J::GetFuzzyMatchingDirectory(std::string path) {
  for (const auto& item : directory_and_weights_map_) {
    std::string last_directory = utils::GetLastDir(item.first);
    if (last_directory.find(path) != std::string::npos) {
      path = item.first;
      break;
    }
  }
  return path;
}
J::J() { ReadHistoryFile(); }
