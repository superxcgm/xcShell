#ifndef INCLUDE_XCSHELL_UTILS_H_
#define INCLUDE_XCSHELL_UTILS_H_

#include <string>
#include <vector>

#include "xcshell/command_executor.h"

namespace utils {
std::vector<std::string> Split(const std::string &str);
std::vector<std::string> SplitArgs(const std::string &str);
std::vector<std::string> SpiltWithSymbol(const std::string &str,
                                         const std::string &symbol);
std::string GetCurrentWorkingDirectory(std::ostream &os_err);
std::string GetLastDir(const std::string &path);
std::string GetHomeDir();
std::string ExpandPath(const std::string &path);
std::string RemoveQuote(const std::string &str);
void PrintSystemError(std::ostream &os_err);
std::string ReadFileText(const std::string &file_name);
std::string GetCommandExecuteResult(CommandExecutor commandExecutor,
                                    const std::string &command);
std::string GetBranchName(const CommandExecutor &commandExecutor);
}  // namespace utils

#endif  // INCLUDE_XCSHELL_UTILS_H_
