#include "sshProxy/configFile.hpp"
#include "config.hpp"
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>
#include <regex>

using std::string;

static std::map<string,string> shortcuts = {
  { "HOME", std::getenv("HOME") }
};

sshProxy::configFile::configFile(std::filesystem::path rawConfigFile) {
  std::filesystem::path configFile;
  std::filesystem::path sysConfigFile = SYSTEM_CONFIG_PATH;
  if (rawConfigFile.c_str()[0] == '~') { // Resolve home references that the shell failed to handle
    logger.debug("'~' in file path assumed to be refering to home, correcting path...");
    std::string filePath = rawConfigFile.string();
    filePath.erase(0, 1);
    std::string homeDir = std::getenv("HOME");
    rawConfigFile = homeDir + filePath;
  }
  if (! rawConfigFile.is_absolute()) {
    logger.debug("File path is not absolute, correcting...");
    configFile = std::filesystem::absolute(rawConfigFile);
  } else {
    configFile = rawConfigFile;
  }
  if (! sysConfigFile.is_absolute()) {
    logger.warn("System file path is not absolute, please check your build system. Proceding anyway...");
    sysConfigFile = std::filesystem::absolute(sysConfigFile);
  }
  try {
    logger.info("Loading system config file: " + sysConfigFile.string());
    toml::table sys = toml::parse_file(sysConfigFile.string());
    toml::table cfg;
    if (std::filesystem::exists(configFile)) {
      logger.info("Loading admin config file: " + configFile.string());
      cfg = toml::parse_file(configFile.string());
    }
    logger.debug("Setting internal configuration");
    #ifdef BUILD_WITH_SSH
    connection = {
      cfg.at_path("connection.username").value_or(sys.at_path("connection.username").ref<string>()),
      cfg.at_path("connection.ipAddr").value_or(sys.at_path("connection.ipAddr").ref<string>()),
      static_cast<int>(cfg.at_path("connection.port").value_or(sys.at_path("connection.port").ref<int64_t>())),
      cfg.at_path("connection.keyFile").value_or(sys.at_path("connection.keyFile").ref<string>())
    };
    #else
    connection = {
      "",
      "",
      0,
      ""
    };
    #endif
    config = {
      static_cast<int>(cfg.at_path("config.clientPort").value_or(sys.at_path("config.clientPort").ref<int64_t>())),
      cfg.at_path("config.openAll").value_or(sys.at_path("config.openAll").ref<bool>()),
      #ifdef BUILD_WITH_SSH
      cfg.at_path("config.compress").value_or(sys.at_path("config.compress").ref<bool>())
      #else
      false
      #endif
    };
  } catch (toml::parse_error& e) {
    logger.fatalStream() << "Toml parsing error!" << e.description();
    exit(1);
  }
  logger.debug("Processing config");
  std::regex regex(CONFIG_SHORTCUT_MATCH);
  // Process keyFile
  if (std::regex_search(connection.keyFile, regex)) {
    std::sregex_iterator begin(connection.keyFile.begin(), connection.keyFile.end(), regex);
    std::sregex_iterator end;
    for (std::sregex_iterator i = begin; i != end; i++) {
      std::smatch match = *i;
      string replace = shortcuts[match[1].str()];
      connection.keyFile.replace(match.position(), match.length(), replace); // Account for the underscores before and after
    }
  }
};