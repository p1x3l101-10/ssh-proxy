#include "ssh-proxy.hpp"
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>

using std::optional;
using std::string;

sshProxy::configFile::configFile(std::filesystem::path configFile) {
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
    logger.fatal("Toml parsing error!");
    std::cerr << e << std::endl;
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
  config.compress     = compress.value_or(false);

  // Choose the client addr
  if (getConfig().openAll) {
    config.clientAddr = "0.0.0.0";
  } else {
    config.clientAddr = "127.0.0.1";
  }
};