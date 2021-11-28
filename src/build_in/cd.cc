#include "xcshell/build_in/cd.h"

#include <unistd.h>

#include <iostream>

#include "xcshell/constants.h"
#include "xcshell/error_handling.h"
#include "xcshell/utils.h"

int Cd::Execute(const std::vector<std::string> &args, std::ostream &os,
                std::ostream &os_err) {
  static std::string pre;
  if (args.size() > 1) {
    os_err << "invalid args" << std::endl;
    return error_code_default;
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
  pre = utils::GetCurrentWorkingDirectory(os_err);
  ErrorHandling::ErrorDispatchHandler(chdir(path.c_str()),
                                      ErrorHandling::ErrorType::NORMAL_ERROR);
  j_.StorageCdHistory(utils::GetCurrentWorkingDirectory(std::cerr));
  return success;
}
