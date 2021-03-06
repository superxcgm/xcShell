#include "xcshell/build_in/build_in.h"

#include "xcshell/build_in/cd.h"
#include "xcshell/build_in/j.h"

BuildIn::BuildIn(std::string_view cd_history) {
  auto j = std::make_shared<J>(cd_history);
  auto cd = std::make_shared<Cd>(*j);
  commands_[cd->GetName()] = cd;
  commands_[j->GetName()] = j;
  commands_[alias_->GetName()] = alias_;
}

bool BuildIn::Exist(const std::string &cmd) {
  return commands_.find(cmd) != commands_.end();
}

int BuildIn::Execute(const std::string &cmd,
                     const std::vector<std::string> &args) {
  return commands_[cmd]->Execute(args, std::cout, std::cerr);
}

std::shared_ptr<Alias> BuildIn::GetAlias() const { return alias_; }
