#include "xcshell/build_in/j.h"

#include <unistd.h>

#include <iostream>

#include "xcshell/constants.h"
#include "xcshell/error_handling.h"
#include "xcshell/utils.h"

int J::Execute(const std::vector<std::string> &args, std::ostream &os,
               std::ostream &os_err) {
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

  std::vector<std::pair<std::string, int>> catalog_and_weights_list =
      utils::ReadFileWithVector(utils::GetCurrentWorkingDirectory(std::cerr));
  for (const auto &item : catalog_and_weights_list) {
    std::string last_directory = utils::GetLastDir(item.first);
    if (last_directory.find(path) != std::string::npos) {
      path = item.first;
      break;
    }
  }

  pre = utils::GetCurrentWorkingDirectory(os_err);
  ErrorHandling::ErrorDispatchHandler(chdir(path.c_str()),
                                      ErrorHandling::ErrorType::NORMAL_ERROR);
  return 0;
}