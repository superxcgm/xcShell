#include "xcshell/error_handling.h"

#include <errno.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "xcshell/constants.h"

void ErrorHandling::SystemCallNoExitOnFailed() {
  ErrorHandling::PrintSystemError();
}

void ErrorHandling::PrintSystemError() {
  std::cerr << strerror(errno) << std::endl;
}

__attribute__((noreturn))
void ErrorHandling::SystemCallExitOnFailed() {
  ErrorHandling::PrintSystemError();
  exit(error_code_default);
}

int ErrorHandling::ErrorDispatchHandler(int return_value,
                                        ErrorType error_type) {
  if (const int error_code_system = -1; return_value == error_code_system) {
    switch (error_type) {
      case ErrorHandling::ErrorType::FATAL_ERROR:
        SystemCallExitOnFailed();
      case ErrorHandling::ErrorType::NORMAL_ERROR:
        SystemCallNoExitOnFailed();
        break;
      default:
        break;
    }
  }
  return return_value;
}
