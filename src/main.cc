<<<<<<< HEAD
=======

>>>>>>> 7d645e6119a85ccd71af87eb63b456b88fe96d85
#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "xcshell/constants.h"
#include "xcshell/shell.h"

int main(int argc, char **argv) {
  CLI::App app("parse the xcShell argv");
  app.add_flag("--no-load-config");
  std::string cd_history;
  app.add_flag("--cd-history-file", cd_history);
  CLI11_PARSE(app, argc, argv);
  bool load_config = app.get_option("--no-load-config")->as<bool>();
  if (cd_history.empty()) {
    cd_history = CD_HISTORY;
  }
<<<<<<< HEAD

  Shell shell(load_config, cd_history);

=======

  Shell shell(load_config,cd_history);

>>>>>>> 7d645e6119a85ccd71af87eb63b456b88fe96d85
  shell.Init();

  shell.Process();

  return shell.Exit();
}
