#ifndef INCLUDE_XCSHELL_UTILS_H_
#define INCLUDE_XCSHELL_UTILS_H_

#include <string>
#include <vector>

#include "xcshell/command_executor.h"

// todo: split this big class
namespace utils {
std::vector<std::string> Split(const std::string &str);
std::vector<std::string> Split(const std::string &str,
                               std::string_view delim);
std::vector<std::string> SplitArgs(const std::string &str);
std::string GetCurrentWorkingDirectory();
std::string GetLastDir(const std::string &path);
std::string GetHomeDir();
std::string ExpandPath(const std::string &path);
std::string RemoveQuote(const std::string &str);
std::string ReadFileText(const std::string &file_name);
std::string LeftTrim(const std::string &str);
std::string RightTrim(const std::string &str);
std::string Trim(const std::string &str);
std::string GetCommandExecuteResult(CommandExecutor *commandExecutor,
                                    const std::string &command);
std::string GetBranchName(CommandExecutor *commandExecutor);
std::string GetRandomString(int len);
std::string GenerateTmpFileName();
std::string GetAbsolutePath(std::string path);
std::string GetDirPath(std::string path);
}  // namespace utils

#endif  // INCLUDE_XCSHELL_UTILS_H_
