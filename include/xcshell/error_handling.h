#ifndef XCSHELL_ERROR_HANDLING_H_
#define XCSHELL_ERROR_HANDLING_H_

#include <iostream>

class ErrorHandling {
 public:
  static int SystemCallNoExitOnFailed(int return_value);
  static int SystemCallExitOnFailed(int return_value);
  static void PrintSystemError(std::ostream& os_err);
};

#endif  // XCSHELL_ERROR_HANDLING_H_
