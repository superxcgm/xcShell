#include "xcshell/build_in/build_in.h"

#include "xcshell/build_in/cd.h"
#include "xcshell/build_in/j.h"

BuildIn::BuildIn() : alias_(new Alias) {
  std::shared_ptr<J> j(new J);
  std::shared_ptr<Cd> cd(new Cd(*j));
  commands_[cd->GetName()] = cd;
  commands_[j->GetName()] = j;
  commands_[alias_->GetName()] = alias_;
}

bool BuildIn::Exist(const std::string& cmd) {
  return commands_.find(cmd) != commands_.end();
}

int BuildIn::Execute(const std::string& cmd,
                     const std::vector<std::string>& args) {
  return commands_[cmd]->Execute(args, std::cout, std::cerr);
}

std::shared_ptr<Alias> BuildIn::GetAlias() { return alias_; }
