#ifndef INCLUDE_XCSHELL_UTILS_H_
#define INCLUDE_XCSHELL_UTILS_H_

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "xcshell/command_executor.h"

namespace utils {
std::vector<std::string> Split(const std::string &str);
std::vector<std::string> Split(const std::string &str,
                               const std::string &delim);
std::vector<std::string> SplitArgs(const std::string &str);
std::string GetCurrentWorkingDirectory(std::ostream &os_err);
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
void StorageDirectoryHistoryInFile(const std::string &pwd);
std::unordered_map<std::string, int> ReadFileWithMap(const std::string &pwd);
void AddCurrentDirectoryInMap(
    const std::string &pwd,
    std::unordered_map<std::string, int> *directory_and_weights_map);
void UpdateDirectoryFileByVector(
    const std::vector<std::pair<std::string, int>> &directory_and_weights_list);
std::vector<std::pair<std::string, int>> SortWithMapValueByVector(
    const std::unordered_map<std::string, int> &directory_and_weights_map);
std::vector<std::pair<std::string, int>> ReadFileWithVector(
    const std::string &pwd);
}  // namespace utils

#endif  // INCLUDE_XCSHELL_UTILS_H_
