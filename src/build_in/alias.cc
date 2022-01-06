#include "xcshell/build_in/alias.h"

#include "xcshell/constants.h"
#include "xcshell/utils.h"

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

  return success;
}

int Alias::PrintAllAlias(std::ostream &os) {
  for (auto const &[k, v] : alias_) {
    PrintSingleAlias(k, os);
  }
  return success;
}

int Alias::PrintSingleAlias(const std::string &name, std::ostream &os) {
  auto it = alias_.find(name);
  if (it == alias_.end()) {
    return success;
  }
  os << name << "=" << it->second << std::endl;
  return success;
}

std::string Alias::Replace(const std::string &cmd) {
  std::string cur_command = cmd;
  std::string tail_str;
  while (alias_.find(cur_command) != alias_.end()) {
    auto v = utils::RemoveQuote(alias_[cur_command]);
    if ((cur_command + " ").append(tail_str).find(v) == 0) {
      break;
    }
    auto pos = v.find(' ');
    if (pos == std::string::npos) {
      AppendString(v, tail_str);
      return v;
    }
    cur_command = v.substr(0, pos);
    auto pre_tail = tail_str;
    tail_str = v.substr(pos + 1);
    AppendString(tail_str, pre_tail);
  }
  AppendString(cur_command, tail_str);
  return cur_command;
}

void Alias::AppendString(std::string &str, std::string_view added_str) const {
  if (added_str.empty()) {
    return;
  }
  str.append(" ");
  str.append(added_str);
}
