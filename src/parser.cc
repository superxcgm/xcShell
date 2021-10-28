#include "xcshell/parser.h"

#include <algorithm>
#include <string>
#include <typeinfo>
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

std::string Parser::ParseEnvironmentVariable(const std::string &arg) {
  std::string parse_arg = arg;
  if (parse_arg.empty() || parse_arg.find('\'') != std::string::npos) {
    parse_arg.erase(std::remove(parse_arg.begin(), parse_arg.end(), '\''),
                    parse_arg.end());
    return parse_arg;
  }

  if (parse_arg.find('$') != std::string::npos) {
    parse_arg.replace(parse_arg.find('$'), 1, "");
    parse_arg =
        getenv(parse_arg.c_str()) == nullptr ? "" : getenv(parse_arg.c_str());
  }
  return parse_arg;
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
      args.push_back(ParseEnvironmentVariable(command_with_arg));
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
