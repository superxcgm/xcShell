#include "../../include/xcshell/build_in/alias.h"

#include "../../include/xcshell/constants.h"
#include "../../include/xcshell/utils.h"

int Alias::Execute(const std::vector<std::string> &args, std::ostream &os,
                   std::ostream &os_err) {
  if (args.empty()) {
    return PrintAllAlias(os);
  }
  std::string definition = args[0];
  auto pos = definition.find('=');
  if (pos == std::string::npos) {
    return PrintSingleAlias(definition, os);
  }
  auto name = definition.substr(0, pos);
  auto value = definition.substr(pos + 1);
  alias_[name] = value;

  return SUCCESS;
}

Alias::~Alias() {}

int Alias::PrintAllAlias(std::ostream &os) {
  for (const auto &item : alias_) {
    PrintSingleAlias(item.first, os);
  }
  return SUCCESS;
}

int Alias::PrintSingleAlias(const std::string &name, std::ostream &os) {
  auto it = alias_.find(name);
  if (it == alias_.end()) {
    return SUCCESS;
  }
  os << name << "=" << it->second << std::endl;
  return SUCCESS;
}

std::string Alias::Replace(const std::string &cmd) {
  return alias_.find(cmd) != alias_.end() ? utils::RemoveQuote(alias_[cmd])
                                          : cmd;
}
