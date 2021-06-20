#ifndef INCLUDE_COMMAND_EXECUTOR_H_
#define INCLUDE_COMMAND_EXECUTOR_H_

#include <string>
#include <vector>

class CommandExecutor {
 public:
  static int Execute(const std::string &command,
                     const std::vector<std::string> &args,
                     std::ostream &err_os);

 private:
  static std::vector<char *> BuildArgv(const std::string command,
                                       const std::vector<std::string> &args);
  static void PrintSystemError(std::ostream &err_os);
  static int ProcessChild(const std::string &command,
                          const std::vector<std::string> &args,
                          std::ostream &err_os);
  static void WaitChildExit(pid_t pid);
};

#endif  // INCLUDE_COMMAND_EXECUTOR_H_
