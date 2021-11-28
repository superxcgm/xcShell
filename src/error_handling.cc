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
  exit(error_code_default);
}

int ErrorHandling::ErrorDispatchHandler(int return_value,
                                        ErrorType error_type) {
  const int error_code_system = -1;
  if (return_value == error_code_system) {
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
