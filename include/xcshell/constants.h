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

#define CD_HISTORY utils::GetHomeDir().append("/.xcShell/cd_history.txt")
#define CD_HISTORY_PATH utils::GetHomeDir().append("/.xcShell")

#endif  // INCLUDE_XCSHELL_CONSTANTS_H_
