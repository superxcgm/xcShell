#include "xcshell/error_handling.h"

#include <errno.h>

#include <iostream>

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

int ErrorHandling::ErrorSelector(int return_value, int error_type) {
  if (return_value == ERROR_CODE_SYSTEM) {
    if (error_type == FATAL_ERROR) {
      SystemCallExitOnFailed();
    } else if (error_type == NORMAL_ERROR) {
      SystemCallNoExitOnFailed();
    }
  }
  return return_value;
}
