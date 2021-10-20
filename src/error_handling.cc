#include "xcshell/error_handling.h"

#include <iostream>

#include "xcshell/constants.h"

int ErrorHandling::SystemCallNoExitOnFailed(int return_value) {
  ErrorHandling::PrintSystemError(std::cerr);
  return return_value;
}

void ErrorHandling::PrintSystemError(std::ostream& os_err) {
  os_err << strerror(errno) << std::endl;
}

int ErrorHandling::SystemCallExitOnFailed() {
  ErrorHandling::PrintSystemError(std::cerr);
  exit(ERROR_CODE_DEFAULT);
}

int ErrorHandling::ErrorSelector(int return_value, int error_type) {
  if (return_value == ERROR_CODE_SYSTEM) {
    if (error_type == FATAL_ERROR) {
      SystemCallExitOnFailed();
    } else if (error_type == NORMAL_ERROR) {
      SystemCallNoExitOnFailed(return_value);
    }
  }
  return return_value;
}
