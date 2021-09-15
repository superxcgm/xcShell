//
// Created by lv qiang on 2021/9/15.
//

#include "xcshell/parse.h"

#include <fcntl.h>

#include <string>
#include <vector>

#include "xcshell/parseTypeElement.h"
#include "xcshell/utils.h"

std::vector<std::string> getArgs(std::vector<std::string> &command_with_args) {
  std::vector<std::string> args;
  for (auto it = 0; it < command_with_args.size(); it++) {
    if (command_with_args[it] == ">" || command_with_args[it] == "<" ||
        command_with_args[it] == ">>") {
      break;
    }
    args.push_back(command_with_args[it]);
  }
  return args;
}

std::string getName(std::vector<std::string> &command_with_args) {
  std::vector<std::string> args;
  for (auto it = 0; it < command_with_args.size();it++) {
    if (command_with_args[it] == ">" || command_with_args[it] == "<" ||
        command_with_args[it] == ">>") {
      int index = ++it;
      if(index < command_with_args.size()) {
        return command_with_args[index];
      }
    }
  }
  return nullptr;
}
int getFlag(std::vector<std::string> &command_with_args) {
  std::vector<std::string> args;
  for (auto it = 0; it < command_with_args.size(); it++) {
    if (command_with_args[it] == "<") {
      return 1;
    } else if (command_with_args[it] == ">") {
      return 2;
    } else if (command_with_args[it] == ">>") {
      return 3;
    }
  }
  return 0;
}

ParseTypeElement Parse::parseUserInputLine(const std::string &input_line) {
  auto parts = utils::SplitArgs(input_line);
  const std::string init_command = parts[0];
  auto command_with_args_str = build_in_.GetAlias()->Replace(init_command);
  auto command_with_args = utils::SplitArgs(command_with_args_str);
  auto command = command_with_args[0];
  parts.erase(parts.begin());
  for (size_t i = command_with_args.size() - 1; i > 0; i--) {
    parts.insert(parts.begin(), command_with_args[i]);
  }
  std::vector<std::string> args;
//  std::string fileName = "a.c";
  if (!parts.empty()) {
    args = getArgs(parts);
//    fileName = getName(parts);
  }

  //  int flag = getFlag(parts);
  std::cout << command << std::endl;
  for (auto it = args.begin(); it != args.end(); it++) {
    std::cout << *it << std::endl;
  }
//  std::cout << fileName << std::endl;
  //return ParseTypeElement(command, args, fileName);
  return ParseTypeElement(command, args);
}
