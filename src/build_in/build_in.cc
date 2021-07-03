#include "../../include/xcshell/build_in/build_in.h"

#include "../../include/xcshell/build_in/cd.h"

BuildIn::BuildIn() {
  std::shared_ptr<Cd> cd(new Cd);
  commands_[cd->GetName()] = cd;
}

bool BuildIn::Exist(const std::string& cmd) {
  return commands_.find(cmd) != commands_.end();
}

int BuildIn::Execute(const std::string& cmd,
                     const std::vector<std::string>& args) {
  return commands_[cmd]->Execute(args);
}
