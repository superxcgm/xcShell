#include "xcshell/parser.h"

#include <algorithm>
#include <string>
#include <vector>

#include "xcshell/CommandParseResult.h"
#include "xcshell/utils.h"
#include "xcshell/constants.h"

CommandParseResult buildParseResultWithRedirect(
    const std::vector<std::string> &command_with_args, const std::string &command) {
  std::vector<std::string> args;
  bool args_end = false;
  std::string output_file;
  std::string input_file;
  bool is_overwrite;
  for (int i = 0; i < command_with_args.size(); i++) {
    auto command_with_arg = command_with_args[i];
    if (command_with_arg == REDIRECT_OUTPUT_OVERWRITE || command_with_arg == REDIRECT_INPUT ||
        command_with_arg == REDIRECT_OUTPUT_APPEND) {
      args_end = true;
    }
    if (!args_end) {
      args.push_back(command_with_arg);
    } else if ((i + 1) < command_with_args.size()) {
      if (command_with_arg == REDIRECT_OUTPUT_OVERWRITE || command_with_arg == REDIRECT_OUTPUT_APPEND) {
        output_file = command_with_args[i + 1];
        is_overwrite = command_with_arg == REDIRECT_OUTPUT_APPEND;
      }
      if (command_with_arg == REDIRECT_INPUT) {
        input_file = command_with_args[i + 1];
      }
    }
  }
  return {command, args, input_file, output_file, is_overwrite};
}

CommandParseResult Parser::ParseUserInputLine(const std::string &input_line) {
  auto [command, commandSuffix] = getCommandAndSuffix(input_line);

  return buildParseResultWithRedirect(commandSuffix, command);
}
std::tuple<std::string, std::vector<std::string>> Parser::getCommandAndSuffix(
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
