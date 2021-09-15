//
// Created by lv qiang on 2021/9/15.
//

#ifndef XCSHELL_PARSETYPEELEMENT_H
#define XCSHELL_PARSETYPEELEMENT_H

#include <string>
#include <utility>
#include <vector>

class ParseTypeElement {
 public:
  ParseTypeElement(std::string command, std::vector<std::string> args,std::string filename,int flag)
      : command(std::move(command)),
        args(std::move(args)),
        filename(std::move(filename)),
        flag(flag) {}

  std::string command;

  std::vector<std::string> args;

  std::string filename;

  int flag = 0;
};

#endif  // XCSHELL_PARSETYPEELEMENT_H
