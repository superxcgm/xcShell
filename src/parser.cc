#include "xcshell/parser.h"

#include <string>
#include <vector>

#include "xcshell/redirectelement.h"
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

std::tuple<std::string, std::vector<std::string>> getCommandAndSuffix(
    const std::string &input_line, BuildIn build_in) {
  auto parts = utils::SplitArgs(input_line);
  const std::string init_command = parts[0];
  auto command_with_args_str = build_in.GetAlias()->Replace(init_command);
  auto command_with_args = utils::SplitArgs(command_with_args_str);
  auto command = command_with_args[0];
  parts.erase(parts.begin());
  for (size_t i = command_with_args.size() - 1; i > 0; i--) {
    parts.insert(parts.begin(), command_with_args[i]);
  }
  return {command, parts};
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

void redirectOutputMode(const std::vector<std::string> &commandSuffix,
                        RedirectElement redirectElement) {
  for (auto &it : commandSuffix) {
    if (it == ">") {
      redirectElement.output_mode = redirectElement.output_mode::overwrite;
    } else if (it == ">>") {
      redirectElement.output_mode = redirectElement.output_mode::append;
    }
  }
}
void getCmdType(const std::vector<std::string> &commandSuffix,
                RedirectElement redirectElement) {
  for (auto &it : commandSuffix) {
    if (it == ">" || it == ">>" || it == "<") {
      redirectElement.cmd_type = redirectElement.cmd_type::direct;
    }
  }
}
RedirectElement Parser::parseUserInputLine(const std::string &input_line) {
  RedirectElement redirectElement;
  auto [command, commandSuffix] = getCommandAndSuffix(input_line, build_in_);

  redirectElement.command = command;
  if (!commandSuffix.empty()) {
    redirectElement.args = getArgs(commandSuffix);
    redirectElement.output_redirect_file = getOutputName(commandSuffix);
    redirectElement.input_redirect_file = getInputName(commandSuffix);
    redirectElement.cmd_type = redirectElement.cmd_type::normal;
    redirectOutputMode(commandSuffix, redirectElement);
    getCmdType(commandSuffix, redirectElement);
  }

  return redirectElement;
}
