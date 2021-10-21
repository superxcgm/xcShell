#ifndef INCLUDE_XCSHELL_ERROR_HANDLING_H_
#define INCLUDE_XCSHELL_ERROR_HANDLING_H_

#include <iostream>

class ErrorHandling {
 private:
  static void SystemCallNoExitOnFailed();
  static void SystemCallExitOnFailed();

 public:
  enum class ErrorType : int { FATAL_ERROR = 1, NORMAL_ERROR = 2 };

  static void PrintSystemError(std::ostream& os_err);
  static int ErrorDispatchHandler(int return_value, ErrorType error_type);
};

#endif  // INCLUDE_XCSHELL_ERROR_HANDLING_H_
