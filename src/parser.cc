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

int GetNonDigitPos(const std::string &environment_variable) {
  int index = 0;
  for (; index < environment_variable.size(); index++) {
    if (!std::isdigit(environment_variable[index])) {
      break;
    }
  }
  if (index == environment_variable.size()) {
    index = 0;
  }
  return index;
}

std::string ErasePrefixNumber(std::string arg) {
  int non_digit_pos = GetNonDigitPos(arg);
  int symbol_index = utils::GetSpecifySymbolPos(arg, '.');
  if (non_digit_pos != 0) {
    return arg.erase(0, non_digit_pos);
  } else if (symbol_index != 0) {
    std::string environment_variable = arg.substr(0, symbol_index);
    std::string extract_environment_variable =
        getenv(environment_variable.c_str()) == nullptr
            ? ""
            : getenv(environment_variable.c_str());
    for (; symbol_index < arg.size(); symbol_index++) {
      extract_environment_variable += arg[symbol_index];
    }
    return extract_environment_variable;
  } else {
    return "";
  }
}

std::string Parser::ExtractEnvironmentVariable(const std::string &arg) {
  std::string environment_variable_parse;
  std::vector<std::string> environment_variable_list = utils::Split(arg, "$");
  if (!environment_variable_list[0].empty()) {
    environment_variable_parse.append(environment_variable_list[0]);
  }
  for (auto it = 1; it < environment_variable_list.size(); it++) {
    std::string environment_variable =
        utils::Trim(environment_variable_list[it]);
    environment_variable_parse =
        getenv(environment_variable.c_str()) == nullptr
            ? environment_variable_parse.append(
                  ErasePrefixNumber(environment_variable))
            : environment_variable_parse.append(
                  getenv(environment_variable.c_str()));
  }
  return environment_variable_parse;
}

std::string Parser::ParseInputArg(const std::string &arg) {
  std::string arg_parse_result;
  if (arg[0] == '\'') {
    arg_parse_result = arg;
    arg_parse_result.erase(
        std::remove(arg_parse_result.begin(), arg_parse_result.end(), '\''),
        arg_parse_result.end());
  } else if (arg.find(PRINTF_EXTRACT) != std::string::npos) {
    arg_parse_result = ExtractEnvironmentVariable(arg);
  } else {
    arg_parse_result = arg;
  }
  return arg_parse_result;
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
  auto parts = utils::SplitArgs(input_line);
  const std::string init_command = parts[0];
  auto command_with_args_str = build_in_.GetAlias()->Replace(init_command);
  auto command_with_args = utils::SplitArgs(command_with_args_str);
  auto command = command_with_args[0];
  parts.erase(parts.begin());
  for (size_t i = command_with_args.size() - 1; i > 0; i--) {
    parts.insert(parts.begin(), command_with_args[i]);
  }
  return {command, parts};
}
