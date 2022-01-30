#include "xcshell/shell.h"

#if defined(__linux__)
#include <readline/history.h>
#include <readline/readline.h>
#elif defined(__APPLE__)
#include <editline/readline.h>
#endif

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <unistd.h>

#include <csignal>
#include <fstream>
#include <sstream>

#include "xcshell/error_handling.h"
#include "xcshell/utils.h"

void Shell::Init() {
  InitLog();
  spdlog::info("Try to init xcShell");
  if (!load_config_) {
    // Todo: extract config class
    const std::string global_config_file = "/etc/xcshrc";
    const std::string user_config_file = "~/.xcshrc";

    auto global_config = utils::ReadFileText(global_config_file);
    spdlog::info("Loading global config: {}", global_config);
    ExecuteConfig(global_config);

    auto user_config = utils::ReadFileText(utils::ExpandPath(user_config_file));
    spdlog::info("Loading user config: {}", user_config);
    ExecuteConfig(user_config);
  }
  IgnoreSignalInterrupt();
  CreateCdHistory(cd_history_);
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
    free(line_ptr);  // NOSONAR
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
  auto pwd = utils::GetCurrentWorkingDirectory();
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
    ErrorHandling::PrintSystemError();
  }
}

void Shell::InitLog() {
  auto logger = spdlog::basic_logger_mt("file_logger", "/tmp/xcShell.log");
  logger->set_level(spdlog::level::debug);
  spdlog::set_default_logger(logger);
  spdlog::flush_every(std::chrono::seconds(3));
}

void Shell::CreateCdHistory(const std::string &path) {
  if (const std::string cd_history_path = utils::GetDirPath(path);
      !path.empty() &&
      access(utils::GetAbsolutePath(cd_history_path).c_str(), F_OK) ==
      std::string::npos) {
    ErrorHandling::ErrorDispatchHandler(
        mkdir(utils::GetAbsolutePath(cd_history_path).c_str(), S_IRWXU),
        ErrorHandling::ErrorType::FATAL_ERROR);
  }
  std::ofstream file(utils::GetAbsolutePath(path));
}
