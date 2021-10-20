#include "xcshell/error_handling.h"

#include <iostream>

#include "xcshell/constants.h"

int ErrorHandling::SystemCallNoExitOnFailed(int return_value) {
  if (return_value == ERROR_CODE_SYSTEM) {
    ErrorHandling::PrintSystemError(std::cerr);
  }
  return return_value;
}

void ErrorHandling::PrintSystemError(std::ostream& os_err) {
  os_err << strerror(errno) << std::endl;
}

int ErrorHandling::SystemCallExitOnFailed(int return_value) {
  if (return_value == ERROR_CODE_SYSTEM) {
    ErrorHandling::PrintSystemError(std::cerr);
    exit(ERROR_CODE_DEFAULT);
  }
  return return_value;
}
