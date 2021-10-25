#ifndef INCLUDE_XCSHELL_CONSTANTS_H_
#define INCLUDE_XCSHELL_CONSTANTS_H_

#include <xcshell/utils.h>

#define SUCCESS 0

#define ERROR_CODE_DEFAULT -1
#define ERROR_CODE_SYSTEM -1

#define REDIRECT_OUTPUT_OVERWRITE ">"
#define REDIRECT_OUTPUT_APPEND ">>"
#define REDIRECT_ERROR_OUTPUT_OVERWRITE "2>"
#define REDIRECT_ERROR_OUTPUT_APPEND "2>>"
#define REDIRECT_ERROR_TO_STDOUT "2>&1"
#define REDIRECT_INPUT "<"
#define REDIRECT_PIPE "|"

#define GLOBAL_CONFIG_FILE "/etc/xcshrc"

#define USER_CONFIG_FILE "~/.xcshrc"

const std::string cd_history =
    utils::GetHomeDir().append("/.xcShell/cd_history.txt");
const std::string cd_history_path = utils::GetHomeDir().append("/.xcShell");

#endif  // INCLUDE_XCSHELL_CONSTANTS_H_
