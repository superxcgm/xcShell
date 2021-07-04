#include "../../include/xcshell/build_in/build_in.h"

#include "../../include/xcshell/build_in/alias.h"
#include "../../include/xcshell/build_in/cd.h"

BuildIn::BuildIn() {
  std::shared_ptr<Cd> cd(new Cd);
  std::shared_ptr<Alias> alias(new Alias);
  commands_[cd->GetName()] = cd;
  commands_[alias->GetName()] = alias;
}

bool BuildIn::Exist(const std::string& cmd) {
  return commands_.find(cmd) != commands_.end();
}

int BuildIn::Execute(const std::string& cmd,
                     const std::vector<std::string>& args) {
  return commands_[cmd]->Execute(args, std::cout, std::cerr);
}
