#include "xcshell/parser.h"

#include <algorithm>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include "xcshell/command_parse_result.h"
#include "xcshell/constants.h"
#include "xcshell/utils.h"

bool Parser::IsRedirect(const std::string &arg) {
  if (arg == REDIRECT_OUTPUT_OVERWRITE || arg == REDIRECT_INPUT ||
      arg == REDIRECT_OUTPUT_APPEND || arg == REDIRECT_ERROR_OUTPUT_OVERWRITE ||
      arg == REDIRECT_ERROR_OUTPUT_APPEND || arg == REDIRECT_ERROR_TO_STDOUT) {
    return true;
  }
  return false;
}

bool Parser::IsErrorToStdoutRedirect(const std::string &arg) {
  if (arg == REDIRECT_ERROR_TO_STDOUT) {
    return true;
  }
  return false;
}

bool Parser::IsOutputRedirectSymbol(const std::string &arg) {
  if (arg == REDIRECT_OUTPUT_OVERWRITE || arg == REDIRECT_OUTPUT_APPEND) {
    return true;
  }
  return false;
}

bool Parser::IsErrorRedirectSymbol(const std::string &arg) {
  if (arg == REDIRECT_ERROR_OUTPUT_OVERWRITE ||
      arg == REDIRECT_ERROR_OUTPUT_APPEND || arg == REDIRECT_ERROR_TO_STDOUT) {
    return true;
  }
  return false;
}

bool Parser::IsInputRedirectSymbol(const std::string &arg) {
  if (arg == REDIRECT_INPUT) {
    return true;
  }
  return false;
}

std::string Parser::ExtractEnvironmentVariable(const std::string &arg, int i) {
  std::string environment_variable;
  std::string environment_variable_suffix;
  std::string environment_variable_result;
  for (; i < arg.size(); i++) {
    if (arg[i] == '$') {
      if (getenv(environment_variable.c_str()) != nullptr) {
        environment_variable_result.append(getenv(environment_variable.c_str()))
            .append(environment_variable_suffix);
      }
      if (std::isdigit(arg[i + 1])) {
        i++;
        for (; i < arg.size(); i++) {
          if (std::isdigit(arg[i])) {
            continue;
          }
          for (; i < arg.size(); i++) {
            environment_variable_suffix += arg[i];
          }
        }
        break;
      }
      environment_variable = "";
      environment_variable_suffix = "";
      continue;
    }
    if (std::isupper(arg[i]) || std::isalpha(arg[i]) || std::isdigit(arg[i]) ||
        arg[i] == '_') {
      environment_variable += arg[i];
      continue;
    } else {
      for (; i < arg.size(); i++) {
        if (arg[i] == '\"') {
          continue;
        }
        if (arg[i] == '$') {
          i--;
          break;
        }
        environment_variable_suffix += arg[i];
      }
    }
  }

  if (getenv(environment_variable.c_str()) != nullptr) {
    environment_variable_result.append(getenv(environment_variable.c_str()))
        .append(environment_variable_suffix);
  } else {
    environment_variable_result.append(environment_variable_suffix);
  }
  return environment_variable_result;
}

std::string Parser::ParseInputArg(const std::string &arg) {
  std::string arg_parse_result;
  std::string arg_parse_result_prefix;
  int i = 0;
  for (; i < arg.size(); i++) {
    if (arg[0] == '\'' && arg[arg.size() - 1] == '\'') {
      arg_parse_result.append(ExtractSingleQuoteString(arg, i));
      break;
    } else if (arg[i] == '$') {
      arg_parse_result.append(ExtractEnvironmentVariable(arg, i));
      break;
    } else if (arg[i] == '\"') {
      i++;
    } else {
      arg_parse_result_prefix += arg[i];
    }
  }
  return arg_parse_result_prefix.append(arg_parse_result);
}

std::string Parser::ExtractSingleQuoteString(const std::string &arg, int i) {
  std::string single_quote_string;
  for (; i < arg.size(); i++) {
    if (arg[i] == '\'') {
      continue;
    }
    single_quote_string += arg[i];
  }
  return single_quote_string;
}

CommandParseResult Parser::BuildParseResultWithRedirect(
    const std::vector<std::string> &origin_args,
    const std::string &command) {
  std::vector<std::string> final_args;
  bool args_end = false;
  bool stderr_is_append = false;
  std::string error_file;
  std::string output_file;
  std::string input_file;
  bool is_append = false;
  for (int i = 0; i < origin_args.size(); i++) {
    auto arg = origin_args[i];
    if (IsRedirect(arg)) args_end = true;
    if (!args_end) {
      final_args.push_back(arg);
    } else if ((i + 1) < origin_args.size()) {
      if (IsOutputRedirectSymbol(arg)) {
        output_file = origin_args[i + 1];
      }
      if (IsInputRedirectSymbol(arg)) {
        input_file = origin_args[i + 1];
      }
      if (IsErrorRedirectSymbol(arg)) {
        error_file = origin_args[i + 1];
      }
    }
    if (arg == REDIRECT_OUTPUT_APPEND) is_append = true;
    if (arg == REDIRECT_ERROR_OUTPUT_APPEND) {
      stderr_is_append = true;
    }
    if (IsErrorToStdoutRedirect(arg)) {
      error_file = output_file;
      stderr_is_append = is_append;
    }
  }
  return {command, final_args, input_file, output_file,
          error_file, is_append, stderr_is_append};
}

std::vector<CommandParseResult> Parser::Parse(
    const std::string &input_line) {
  std::vector<CommandParseResult> commands;
  std::vector<std::string> commands_str =
      utils::Split(input_line, REDIRECT_PIPE);
  for (const auto &command_str : commands_str) {
    auto [command_name, args] = ParseCommand(command_str);

    commands.push_back(BuildParseResultWithRedirect(args, command_name));
  }
  return commands;
}

std::tuple<std::string, std::vector<std::string>> Parser::ParseCommand(
    const std::string &input_line) {
  auto [init_command_name, args_str] = SplitCommandNameAndArgs(input_line);
//  spdlog::debug("init_command_name: {}, args_str: {}", init_command_name);
  auto alias_command = build_in_.GetAlias()->Replace(init_command_name);
  auto [command_name, extra_args_str] = SplitCommandNameAndArgs(alias_command);
  auto args = SplitArgs(args_str + " " + extra_args_str);
  return {command_name, args};
}

int NextNonSpacePos(int start, const std::string &str) {
  int i = start;
  while (i < str.length() && str[i] == ' ') i++;
  return i;
}

std::pair<std::string, std::string> Parser::SplitCommandNameAndArgs(const std::string& command) {
  int i = NextNonSpacePos(0, command);
  int j;
  for (j = i + 1; j < command.size(); j++) {
    if (command[j] == ' ') {
      break;
    }
  }
  std::string command_name = command.substr(i, j - i);
  std::string args;
  if (j + 1 < command.size()) {
    args = command.substr(j + 1);
  }
  return {command_name, args};
}

std::string ExtractQuoteString(int start, char quotation_mark,
                               const std::string &str, std::ostream &os_err) {
  int i;
  int quotation_mark_count = 1;
  for (i = start; i < str.length(); i++) {
    if (str[i] == quotation_mark) {
      // todo: ignore continuous quotation mark
      if (i + 1 < str.length() && str[i + 1] == quotation_mark) {
        quotation_mark_count += 2;
        i++;
        continue;
      }
      quotation_mark_count++;
      break;
    }
  }
  if (quotation_mark_count % 2 != 0) {
    // todo: error handle: abort execute, failed
    os_err << "quotation mark do not match" << std::endl;
  }
  return str.substr(start, i - start);
}

std::string ExtractStringWithoutQuote(int start, const std::string &str) {
  int i = start;
  for (; i < str.length(); i++) {
    if (str[i] == ' ') {
      break;
    }
    if (str[i - 1] == '=' && (str[i] == '\'' || str[i] == '"')) {
      return str.substr(start, i - start + 1) +
             ExtractQuoteString(i + 1, str[i], str, std::cerr) + str[i];
    }
  }
  return str.substr(start, i - start);
}

std::vector<std::string> Parser::SplitArgs(const std::string &str) {
  if (str.empty()) {
    return {};
  }
  std::vector<std::string> args;
  int i = NextNonSpacePos(0, str);
  for (; i < str.length();) {
    std::string fragment;
    if (str[i] == QUOTATION_MARK_DOUBLE) {
      fragment = std::move(ExtractQuoteString(i + 1, str[i], str, std::cerr));
      i += 2;  // ignore quotation mark
    } else if (str[i] == QUOTATION_MARK_SINGLE) {
      fragment +=
          '\'' + ExtractQuoteString(i + 1, str[i], str, std::cerr) + '\'';
    } else {
      fragment = std::move(ExtractStringWithoutQuote(i, str));
    }
    args.push_back(ParseInputArg(fragment));
    i = NextNonSpacePos(i + fragment.size(), str);
  }
  if (i < str.length()) {
    args.push_back(str.substr(i));
  }

  return args;
}
