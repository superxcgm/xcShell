#include "xcshell/build_in/j.h"

#include <fcntl.h>
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
  directory_and_weights_map[path]++;
  directory_and_weights_list = SortWithMapValueByVector();
  UpdateDirectoryFileByVector();
}

std::vector<std::pair<std::string, int>> J::SortWithMapValueByVector() {
  std::vector<std::pair<std::string, int>> directory_and_weights(
      directory_and_weights_map.begin(), directory_and_weights_map.end());
  sort(directory_and_weights.begin(), directory_and_weights.end(),
       [](const std::pair<std::string, int>& x,
          const std::pair<std::string, int>& y) -> int {
         return x.second > y.second;
       });
  return directory_and_weights;
}

void J::ReadHistoryFile() {
  CreateCdHistory();
  char buf[BUFSIZ];
  std::ifstream in(cd_history.c_str());
  int line = 0;
  while (in.getline(buf, BUFSIZ)) {
    std::vector<std::string> directory_and_weights = utils::Split(buf, " ");
    directory_and_weights_map.insert(std::make_pair(
        directory_and_weights[0], atoi(directory_and_weights[1].c_str())));
    line++;
  }
}

void J::CreateCdHistory() const {
  if (access(cd_history_path.c_str(), F_OK) == -1) {
    ErrorHandling::ErrorDispatchHandler(mkdir(cd_history_path.c_str(), S_IRWXU),
                                        ErrorHandling::ErrorType::FATAL_ERROR);
    std::ofstream file(cd_history.c_str());
  }
}

void J::UpdateDirectoryFileByVector() {
  std::ofstream file_empty(cd_history.c_str(), std::ios_base::out);
  std::ofstream update_file(cd_history, std::ios::app);
  auto item = directory_and_weights_list.begin();
  for (; item != directory_and_weights_list.end(); item++) {
    update_file << item->first + " " << item->second << std::endl;
  }
  update_file.close();
}

std::string J::GetFuzzyMatchingDirectory(std::string path) {
  for (const auto& item : directory_and_weights_list) {
    std::string last_directory = utils::GetLastDir(item.first);
    if (last_directory.find(path) != std::string::npos) {
      path = item.first;
      break;
    }
  }
  return path;
}
J::J() {
  ReadHistoryFile();
  directory_and_weights_list = SortWithMapValueByVector();
}
