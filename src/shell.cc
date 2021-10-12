#include "xcshell/shell.h"

#if defined(__linux__)
#include <readline/history.h>
#include <readline/readline.h>
#elif defined(__APPLE__)
#include <editline/readline.h>
#endif

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <sstream>

#include "xcshell/constants.h"
#include "xcshell/utils.h"

void Shell::Init(int argc, char **argv) {
  InitLog();
  spdlog::info("Try to init xcShell");
  bool load_config = true;
  if (argc > 1) {
    std::string option = argv[1];
    if (option == "--no-load-config") {
      load_config = false;
    }
  }

  if (load_config) {
    auto global_config = utils::ReadFileText(GLOBAL_CONFIG_FILE);
    spdlog::info("Loading global config: {}", global_config);
    ExecuteConfig(global_config);

    auto user_config = utils::ReadFileText(utils::ExpandPath(USER_CONFIG_FILE));
    spdlog::info("Loading user config: {}", user_config);
    ExecuteConfig(user_config);
  }

  IgnoreSignalInterrupt();
}

void Shell::Process() {
  while (true) {
    char *line_ptr;
    line_ptr = readline(GeneratePrompt().c_str());
    if (line_ptr == nullptr) {
      break;
    }
    if (strlen(line_ptr) > 0) {
      spdlog::info("Get user input: {}", line_ptr);
      add_history(line_ptr);
    }
    std::string line = line_ptr;
    free(line_ptr);
    if (line.empty()) {
      continue;
    }
    command_executor_.Execute(line);
  }
  spdlog::default_logger()->flush();
}

int Shell::Exit() {
  // todo: do necessary clean up
  return 0;
}

std::string Shell::GeneratePrompt() {
  auto pwd = utils::GetCurrentWorkingDirectory(std::cerr);
  auto home = utils::GetHomeDir();
  std::string prompt_line;
  if (home == pwd) {
    prompt_line = "~";
  } else {
    prompt_line = utils::GetLastDir(pwd);
  }
  prompt_line.append(utils::GetBranchName(&command_executor_));
  return prompt_line + " > ";
}

void Shell::ExecuteConfig(const std::string &config_string) {
  if (config_string.empty()) {
    return;
  }
  std::istringstream ss(config_string);

  while (!ss.eof()) {
    std::string line;
    getline(ss, line);
    if (line.empty()) {
      continue;
    }
    command_executor_.Execute(line);
  }
}

void Shell::IgnoreSignalInterrupt() {
  struct sigaction new_action {};
  new_action.sa_handler = SIG_IGN;
  int result = sigaction(SIGINT, &new_action, nullptr);
  if (result) {
    utils::PrintSystemError(std::cerr);
  }
}

void Shell::InitLog() {
  auto logger = spdlog::basic_logger_mt("file_logger", "/tmp/xcShell.log");
  logger->set_level(spdlog::level::debug);
  spdlog::set_default_logger(logger);
  spdlog::flush_every(std::chrono::seconds(3));
}
