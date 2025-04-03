#include "ssh-proxy.hpp"
#include "config.hpp"

ssh::Session* sshProxy::createSession (configFile config) {
  log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".createSession");
  try {
    logger.info("Creating Session");
    ssh::Session* session;

    logger.debug("Setting variables");
    session->setOption(SSH_OPTIONS_HOST, config.getConnection().ipAddr.c_str());
    session->setOption(SSH_OPTIONS_USER, config.getConnection().username.c_str());
    session->setOption(SSH_OPTIONS_PORT, config.getConnection().port);

    logger.info("Connecting");
    session->connect();

    if (! config.getConnection().keyFile.empty()) {
      logger.info("Authorizing using configured key");
      ssh_key key;
      ssh_pki_import_privkey_file(config.getConnection().keyFile.c_str(), nullptr, nullptr, nullptr, &key);
      session->userauthPublickey(key);
    } else {
      logger.info("Attempting to detect key");
      session->userauthPublickeyAuto();
    }
    logger.debug("Session created");
    return session;
  } catch (ssh::SshException& e) {
    logger.fatal("SSH Error: " + e.getError());
    exit(e.getCode());
  }
}