#include "../../include/xcshell/build_in/cd.h"

#include <unistd.h>

#include <iostream>

#include "../../include/xcshell/error.h"
#include "../../include/xcshell/utils.h"

int Cd::Execute(const std::vector<std::string> &args) {
  static std::string pre;
  if (args.size() > 1) {
    std::cerr << "invalid args" << std::endl;
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

  pre = utils::GetCurrentWorkingDirectory();

  int ret = chdir(path.c_str());
  if (ret == ERROR_CODE_SYSTEM) {
    utils::PrintSystemError();
    return ERROR_CODE_DEFAULT;
  }
  return 0;
}
