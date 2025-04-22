#include "socks5Values/address.hpp"
#include "sshProxy/socks5Session.hpp"
#include "socks5Values/connectResponce.hpp"
#include "loggerMacro.hpp"
#include <memory>

void sshProxy::socks5Session::connectSsh(socks5Values::clientConnect &connection) {
  createLogger(logger);
  auto self(shared_from_this());
  enum socks5Values::responceStatus status;
  // Create ssh channel
  channel = std::make_shared<ssh::Channel>(*session);
  logger.debug("Created channel");
  // Open tunnel
  try {
    channel->openForward(connection.destinationAddress.string().c_str(), connection.destinationPort.portNum, config->getConfig().clientAddr.c_str(), config->getConfig().clientPort);
    logger.info("Connection successful");
    status = socks5Values::responceStatus::GRANTED;
  } catch (ssh::SshException &e) {
    logger.errorStream() << "Failed to open SSH tunnel: " << e.getError();

    status = socks5Values::responceStatus::GENERAL_FAILURE;
  }
  // Register remote socket (via ssh channel)
  logger.debug("Created remote socket");
  // Start connection
  startClientToRemoteRelay();
  startRemoteToClientRelay();  
}