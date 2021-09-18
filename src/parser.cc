#include "xcshell/parser.h"

#include <algorithm>
#include <string>
#include <vector>

#include "xcshell/CommandParseResult.h"
#include "xcshell/utils.h"

std::vector<std::string> getArgs(
    const std::vector<std::string> &command_with_args) {
  std::vector<std::string> args;
  for (auto &command_with_arg : command_with_args) {
    if (command_with_arg == ">" || command_with_arg == "<" ||
        command_with_arg == ">>") {
      break;
    }
    args.push_back(command_with_arg);
  }
  return args;
}

std::string getOutputName(const std::vector<std::string> &commandSuffix) {
  for (auto it = 0; it < commandSuffix.size(); it++) {
    if (commandSuffix[it] == ">" || commandSuffix[it] == ">>") {
      int index = ++it;
      if (index < commandSuffix.size()) {
        return commandSuffix[index];
      }
    }
  }
  return "";
}

std::string getInputName(const std::vector<std::string> &commandSuffix) {
  for (auto it = 0; it < commandSuffix.size(); it++) {
    if (commandSuffix[it] == "<") {
      int index = ++it;
      if (index < commandSuffix.size()) {
        return commandSuffix[index];
      }
    }
  }
  return "";
}

bool redirectOutputMode(const std::vector<std::string> &commandSuffix) {
  if (std::any_of(commandSuffix.begin(), commandSuffix.end(),
                  [](const std::string &symbol) { return symbol == ">>"; })) {
    return true;
  }
  return false;
}
CommandParseResult Parser::ParseUserInputLine(const std::string &input_line) {
  CommandParseResult command_parse_result;
  auto [command, commandSuffix] = getCommandAndSuffix(input_line);

  command_parse_result.command = command;
  if (!commandSuffix.empty()) {
    command_parse_result.args = getArgs(commandSuffix);
    command_parse_result.output_redirect_file = getOutputName(commandSuffix);
    command_parse_result.input_redirect_file = getInputName(commandSuffix);
    command_parse_result.output_is_append = redirectOutputMode(commandSuffix);
  }

  return command_parse_result;
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
