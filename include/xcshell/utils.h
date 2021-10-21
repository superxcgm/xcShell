#ifndef INCLUDE_XCSHELL_UTILS_H_
#define INCLUDE_XCSHELL_UTILS_H_

#include <string>
#include <unordered_map>
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
void StorageCatalogHistoryInFile(const std::string &pwd);
std::unordered_map<std::string, int> ReadFileWithMap(
    const std::string &xcShell_storage_catalog_file, const std::string &pwd);
void InsertCurrentCatalogInMap(
    const std::string &pwd,
     std::unordered_map<std::string, int> *catalog_and_weights_map);
void UpdateCatalogFileByMap(
    const std::string &xcShell_storage_catalog_file,
    const std::unordered_map<std::string, int> &catalog_and_weights_map);
}  // namespace utils

#endif  // INCLUDE_XCSHELL_UTILS_H_
