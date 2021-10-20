#ifndef INCLUDE_XCSHELL_ERROR_HANDLING_H_
#define INCLUDE_XCSHELL_ERROR_HANDLING_H_

#include <iostream>

class ErrorHandling {
 private:
  static void SystemCallNoExitOnFailed();
  static void SystemCallExitOnFailed();

 public:
  static void PrintSystemError(std::ostream& os_err);
  static int ErrorSelector(int return_value, int error_type);
};

#endif  // INCLUDE_XCSHELL_ERROR_HANDLING_H_
