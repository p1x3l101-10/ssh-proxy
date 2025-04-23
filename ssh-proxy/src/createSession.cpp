#include "sshProxy/createSession.hpp"
#include "config.hpp"
#include <memory>
#include <string>

bool sshReachable = true;

std::shared_ptr<ssh::Session> sshProxy::createSession (std::shared_ptr<configFile> config) {
  log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".createSession");
  try {
    logger.info("Creating Session");
    std::shared_ptr<ssh::Session> session(new ssh::Session);

    logger.debug("Setting variables");
    session->setOption(SSH_OPTIONS_HOST, config->getConnection().ipAddr.c_str());
    logger.debug("Connection to address: <SCRUBBED>");
    session->setOption(SSH_OPTIONS_USER, config->getConnection().username.c_str());
    logger.debug("Connecting as user: " + config->getConnection().username);
    session->setOption(SSH_OPTIONS_PORT, config->getConnection().port);
    logger.debug("Connecting on port: " + std::to_string(config->getConnection().port));

    logger.info("Starting session");
    try {
      session->connect();
    } catch (ssh::SshException &e) {
      if (e.getError().contains("Timeout connecting to " + config->getConnection().ipAddr)) {
        logger.emerg("Unable to connect to the ssh server, expect blocks");
        sshReachable = false;
        return nullptr;
      } else {
        throw e;
      }
    }
    sshReachable = true;

    if (! config->getConnection().keyFile.empty()) {
      logger.info("Authorizing using configured key");
      ssh_key key;
      ssh_pki_import_privkey_file(config->getConnection().keyFile.c_str(), nullptr, nullptr, nullptr, &key);
      int ret = session->userauthPublickey(key);
      if (ret != SSH_AUTH_SUCCESS) {
        logger.fatal("Failed to authorize with public key");
        exit(1);
      }
    } else {
      logger.info("Attempting to detect key");
      int ret = session->userauthPublickeyAuto();
      if (ret != SSH_AUTH_SUCCESS) {
        logger.fatal("Failed to authorize with public key");
        exit(1);
      }
    }
    logger.debug("Session created");
    return session;
  } catch (ssh::SshException& e) {
    logger.fatal("SSH Error: " + e.getError());
    exit(e.getCode());
  }
}