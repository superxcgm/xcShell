#include "xcshell/build_in/cd.h"

#include <unistd.h>

#include <iostream>

#include "xcshell/constants.h"
#include "xcshell/utils.h"

int Cd::Execute(const std::vector<std::string> &args, std::ostream &os,
                std::ostream &os_err) {
  static std::string pre;
  if (args.size() > 1) {
    os_err << "invalid args" << std::endl;
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

  pre = utils::GetCurrentWorkingDirectory(os_err);

  utils::SystemCallNotExitOnFailed(chdir(path.c_str()));
  return 0;
}
