#include "xcshell/error_handling.h"

#include <errno.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "xcshell/constants.h"

void ErrorHandling::SystemCallNoExitOnFailed() {
  ErrorHandling::PrintSystemError(std::cerr);
}

void ErrorHandling::PrintSystemError(std::ostream& os_err) {
  os_err << strerror(errno) << std::endl;
}

void ErrorHandling::SystemCallExitOnFailed() {
  ErrorHandling::PrintSystemError(std::cerr);
  exit(ERROR_CODE_DEFAULT);
}

int ErrorHandling::ErrorDispatchHandler(int return_value,
                                        ErrorType error_type) {
  if (return_value == ERROR_CODE_SYSTEM) {
    switch (error_type) {
      case ErrorHandling::ErrorType::FATAL_ERROR:
        SystemCallExitOnFailed();
        break;
      case ErrorHandling::ErrorType::NORMAL_ERROR:
        SystemCallNoExitOnFailed();
        break;
      default:
        break;
    }
  }
  return return_value;
}
