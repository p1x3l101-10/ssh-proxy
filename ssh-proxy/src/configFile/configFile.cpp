#include "sshProxy/configFile.hpp"
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>

using std::optional;
using std::string;

sshProxy::configFile::configFile(std::filesystem::path rawConfigFile) {
  std::filesystem::path configFile;
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
  logger.info("Serializing config file: " + configFile.string());
  optional<string> username;
  optional<string> ipAddr;
  optional<int> port;
  optional<string> keyFile;
  optional<int> clientPort;
  optional<bool> openAll;
  optional<bool> compress;
  try {
    logger.debug("Loading toml table");
    toml::table tbl = toml::parse_file(configFile.string());
    logger.debug("Setting optionals from table");
    username   = tbl.at_path("connection.username").value<string>();
    ipAddr     = tbl.at_path("connection.ipAddr")  .value<string>();
    port       = tbl.at_path("connection.port")    .value<int>   ();
    keyFile    = tbl.at_path("connection.keyFile") .value<string>();
    clientPort = tbl.at_path("config.clientPort")  .value<int>   ();
    openAll    = tbl.at_path("config.openAll")     .value<bool>  ();
    compress   = tbl.at_path("config.compress")    .value<bool>  ();
  } catch (toml::parse_error& e) {
    logger.fatalStream() << "Toml parsing error!" << e.description();
    exit(1);
  }
  logger.debug("Ensuring config has required variables");
  if ( // If the required values are empty
    ! username.has_value() ||
    ! ipAddr.has_value() ||
    ! port.has_value()
  ) {
    logger.fatal("Malformed configuration file!");
    logger.fatal("Missing required values");
    exit(1);
  }
  logger.debug("Setting internal configuration");
  connection.username = username.value();
  connection.ipAddr   = ipAddr.value();
  connection.port     = port.value();
  connection.keyFile  = keyFile.value_or("");
  config.clientPort   = clientPort.value_or(1080);
  config.openAll      = openAll.value_or(false);
  config.compress     = compress.value_or(false); // TODO: Actually impliment this...

  // Choose the client addr
  if (getConfig().openAll) {
    config.clientAddr = "0.0.0.0";
  } else {
    config.clientAddr = "127.0.0.1";
  }
};