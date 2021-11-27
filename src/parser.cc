#include "xcshell/parser.h"

#include <algorithm>
#include <string>
#include <vector>

#include "xcshell/command_parse_result.h"
#include "xcshell/utils.h"

std::string Parser::redirect_output_overwrite_ = ">";
std::string Parser::redirect_output_append_ = ">>";
std::string Parser::redirect_error_output_overwrite_ = "2>";
std::string Parser::redirect_error_output_append_ = "2>>";
std::string Parser::redirect_error_to_stdout_ = "2>&1";
std::string Parser::redirect_input_ = "<";
std::string Parser::redirect_pipe_ = "|";
char Parser::quotation_mark_single_ = '\'';
char Parser::quotation_mark_double_ = '"';

bool Parser::IsRedirect(const std::string &arg) {
  if (arg == redirect_output_overwrite_ || arg == redirect_input_ ||
      arg == redirect_output_append_ || arg == redirect_error_output_overwrite_ ||
      arg == redirect_error_output_append_ || arg == redirect_error_to_stdout_) {
    return true;
  }
  return false;
}

bool Parser::IsErrorToStdoutRedirect(const std::string &arg) {
  if (arg == redirect_error_to_stdout_) {
    return true;
  }
  return false;
}

bool Parser::IsOutputRedirectSymbol(const std::string &arg) {
  if (arg == redirect_output_overwrite_ || arg == redirect_output_append_) {
    return true;
  }
  return false;
}

bool Parser::IsErrorRedirectSymbol(const std::string &arg) {
  if (arg == redirect_error_output_overwrite_ ||
      arg == redirect_error_output_append_ || arg == redirect_error_to_stdout_) {
    return true;
  }
  return false;
}

bool Parser::IsInputRedirectSymbol(const std::string &arg) {
  if (arg == redirect_input_) {
    return true;
  }
  return false;
}

CommandParseResult Parser::BuildParseResultWithRedirect(
    const std::vector<std::string> &origin_args, const std::string &command) {
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
    if (arg == redirect_output_append_) is_append = true;
    if (arg == redirect_error_output_append_) {
      stderr_is_append = true;
    }
    if (IsErrorToStdoutRedirect(arg)) {
      error_file = output_file;
      stderr_is_append = is_append;
    }
  }
  return {command,    final_args, input_file,      output_file,
          error_file, is_append,  stderr_is_append};
}

std::optional<std::vector<CommandParseResult>> Parser::Parse(
    const std::string &input_line) {
  std::vector<CommandParseResult> commands;
  std::vector<std::string> commands_str =
      utils::Split(input_line, redirect_pipe_);
  for (const auto &command_str : commands_str) {
    auto maybeCommandAndArgs = ParseCommand(command_str);
    if (!maybeCommandAndArgs.has_value()) {
      return {};
    }
    auto [command_name, args] = maybeCommandAndArgs.value();
    commands.push_back(BuildParseResultWithRedirect(args, command_name));
  }
  return commands;
}

std::optional<std::tuple<std::string, std::vector<std::string>>>
Parser::ParseCommand(const std::string &input_line) {
  auto [init_command_name, args_str] = SplitCommandNameAndArgs(input_line);
  //  spdlog::debug("init_command_name: {}, args_str: {}", init_command_name);
  auto alias_command = build_in_.GetAlias()->Replace(init_command_name);
  auto [command_name, extra_args_str] = SplitCommandNameAndArgs(alias_command);
  auto maybe_args = SplitArgs(args_str + " " + extra_args_str);
  if (!maybe_args.has_value()) {
    return {};
  }
  return std::tuple<std::string, std::vector<std::string>>(command_name,
                                                           maybe_args.value());
}

int Parser::NextNonSpacePos(int start, const std::string &str) {
  int i = start;
  while (i < str.length() && str[i] == ' ') i++;
  return i;
}

std::pair<std::string, std::string> Parser::SplitCommandNameAndArgs(
    const std::string &command) {
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

std::optional<std::pair<std::string, int>> Parser::ExtractQuoteString(
    int start, char quotation_mark, const std::string &str,
    std::ostream &os_err) {
  int i;
  int quotation_mark_count = 1;
  std::string ans;
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
    ans += str[i];
  }
  if (quotation_mark_count % 2 != 0) {
    os_err << "quotation mark do not match" << std::endl;
    return {};
  }
  return std::pair(ans, i + 1);
}

std::optional<std::pair<std::string, int>> Parser::ExtractStringWithoutQuote(
    int start, const std::string &str) {
  int i = start;
  for (; i < str.length(); i++) {
    if (str[i] == ' ') {
      break;
    }
    if (str[i - 1] == '=' &&
        (str[i] == quotation_mark_single_ || str[i] == quotation_mark_double_)) {
      // todo: refactor
      auto maybeValue = ExtractQuoteString(i + 1, str[i], str, std::cerr);
      if (!maybeValue.has_value()) {
        return {};
      }
      auto [value, next] = maybeValue.value();
      return std::pair(str.substr(start, i - start + 1) + value + str[i], next);
    }
  }
  return std::pair(str.substr(start, i - start), i);
}

std::optional<std::vector<std::string>> Parser::SplitArgs(
    const std::string &str) {
  if (str.empty()) {
    return std::vector<std::string>();
  }
  std::vector<std::string> args;
  int i = NextNonSpacePos(0, str);
  for (; i < str.length();) {
    std::string fragment;
    bool complete_quote = str[i] == quotation_mark_single_;

    if (str[i] == quotation_mark_double_ || str[i] == quotation_mark_single_) {
      auto maybe_value = ExtractQuoteString(i + 1, str[i], str, std::cerr);
      if (!maybe_value.has_value()) {
        return {};
      }
      fragment = maybe_value.value().first;
      i = maybe_value.value().second;
    } else {
      auto maybe_value = ExtractStringWithoutQuote(i, str);
      if (!maybe_value.has_value()) {
        return {};
      }
      auto [value, next] = maybe_value.value();
      fragment = value;
      i = next;
    }
    if (!complete_quote) {
      fragment = ReplaceVariable(fragment);
    }
    args.push_back(fragment);
    i = NextNonSpacePos(i, str);
  }
  if (i < str.length()) {
    args.push_back(str.substr(i));
  }

  return args;
}
std::pair<std::string, int> Parser::ExtractVariable(const std::string &str,
                                                    int start) {
  int left = start;
  if (std::isalpha(str[start]) || str[start] == '_') {
    int right = start + 1;
    while (right < str.size() && std::isalnum(str[right]) ||
           str[right] == '_') {
      right++;
    }
    std::string variable_name = str.substr(left, right - left);
    char *value = getenv(variable_name.c_str());
    if (value == nullptr) {
      return {"", right};
    } else {
      return {value, right};
    }
  }
  return {"", start};
}

std::string Parser::ReplaceVariable(const std::string &str) {
  std::string ans;
  for (int i = 0; i < str.size();) {
    if (str[i] == '$') {
      auto [value, next] = ExtractVariable(str, i + 1);
      ans += value;
      i = next;
    } else {
      ans += str[i];
      i++;
    }
  }
  return ans;
}
