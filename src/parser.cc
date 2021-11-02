#include "xcshell/parser.h"

#include <algorithm>
#include <string>
#include <vector>

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
    const std::vector<std::string> &command_with_args,
    const std::string &command) {
  std::vector<std::string> args;
  bool args_end = false;
  bool stderr_is_append = false;
  std::string error_file;
  std::string output_file;
  std::string input_file;
  bool is_append = false;
  for (int i = 0; i < command_with_args.size(); i++) {
    auto command_with_arg = command_with_args[i];
    if (IsRedirect(command_with_arg)) args_end = true;
    if (!args_end) {
      args.push_back(ParseInputArg(command_with_arg));
    } else if ((i + 1) < command_with_args.size()) {
      if (IsOutputRedirectSymbol(command_with_arg)) {
        output_file = command_with_args[i + 1];
      }
      if (IsInputRedirectSymbol(command_with_arg)) {
        input_file = command_with_args[i + 1];
      }
      if (IsErrorRedirectSymbol(command_with_arg)) {
        error_file = command_with_args[i + 1];
      }
    }
    if (command_with_arg == REDIRECT_OUTPUT_APPEND) is_append = true;
    if (command_with_arg == REDIRECT_ERROR_OUTPUT_APPEND) {
      stderr_is_append = true;
    }
    if (IsErrorToStdoutRedirect(command_with_arg)) {
      error_file = output_file;
      stderr_is_append = is_append;
    }
  }
  return {command,    args,      input_file,      output_file,
          error_file, is_append, stderr_is_append};
}

std::vector<CommandParseResult> Parser::ParseUserInputLine(
    const std::string &input_line) {
  std::vector<CommandParseResult> command_parse_result_list;
  std::vector<std::string> command_and_suffix_list =
      utils::Split(input_line, REDIRECT_PIPE);
  for (const auto &command_and_suffix : command_and_suffix_list) {
    auto [command, commandSuffix] =
        GetCommandAndSuffix(utils::Trim(command_and_suffix));

    command_parse_result_list.push_back(
        BuildParseResultWithRedirect(commandSuffix, command));
  }
  return command_parse_result_list;
}

std::tuple<std::string, std::vector<std::string>> Parser::GetCommandAndSuffix(
    const std::string &input_line) {
  auto parts = SplitArgs(input_line);
  const std::string init_command = parts[0];
  auto command_with_args_str = build_in_.GetAlias()->Replace(init_command);
  auto command_with_args = SplitArgs(command_with_args_str);
  auto command = command_with_args[0];
  parts.erase(parts.begin());
  for (size_t i = command_with_args.size() - 1; i > 0; i--) {
    parts.insert(parts.begin(), command_with_args[i]);
  }
  return {command, parts};
}

int NextNonSpacePos(int start, const std::string &str) {
  int i = start;
  while (i < str.length() && str[i] == ' ') i++;
  return i;
}

std::string ExtractQuoteString(int start, char quotation_mark,
                               const std::string &str, std::ostream &os_err) {
  int i;
  int check_number = 1;
  for (i = start; i < str.length(); i++) {
    if (str[i] == quotation_mark) {
      check_number++;
      break;
    }
  }
  if (check_number % 2 != 0) {
    os_err << "less one quote" << std::endl;
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
  std::vector<std::string> parts;
  int i = NextNonSpacePos(0, str);
  std::vector<char> quotation_marks = {'\'', '"'};
  for (; i < str.length();) {
    std::string fragment;
    if (str[i] == quotation_marks[1]) {
      fragment = std::move(ExtractQuoteString(i + 1, str[i], str, std::cerr));
      i += 2;  // ignore quotation mark
    } else if (str[i] == quotation_marks[0]) {
      fragment +=
          '\'' + ExtractQuoteString(i + 1, str[i], str, std::cerr) + '\'';
    } else {
      fragment = std::move(ExtractStringWithoutQuote(i, str));
    }
    parts.push_back(fragment);
    i = NextNonSpacePos(i + fragment.size(), str);
  }
  if (i < str.length()) {
    parts.push_back(str.substr(i));
  }

  return parts;
}
