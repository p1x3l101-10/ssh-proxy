#include "socks5Values/address.hpp"
#include "sshProxy/socks5Session.hpp"
#include "socks5Values/connectResponce.hpp"
#include "loggerMacro.hpp"
#include <memory>
#include <magic_enum/magic_enum.hpp>
#include "sshProxy/sshSocket.hpp"

void sshProxy::socks5Session::connectSsh(socks5Values::clientConnect &connection) {
  createLogger(logger);
  auto self(shared_from_this());
  logger.debugStream() << "Connection diagram - IP: " << connection.destinationAddress.string() << ", Port: " << connection.destinationPort.string() << ", Address Type: " << magic_enum::enum_name(connection.destinationAddress.type);
  // Override the socket
  try {
    remoteSocket = std::make_shared<sshProxy::sshSocket>(clientSocket.get_executor(), *session);
  } catch (ssh::SshException &e) {
    logger.errorStream() << "Failed to open SSH tunnel: " << e.getError();
    socks5Values::connectResponce failure = socks5Values::responceStatus::NETWORK_UNREACHABLE; // SSH server unreachable
    auto ret = async_write(clientSocket, boost::asio::buffer(failure.data()));
    closeBoth(); // Shut down this controlpath
  }
  logger.debug("Created ssh remote socket");
  // Start connection
  startClientToRemoteRelay();
  startRemoteToClientRelay();  
}