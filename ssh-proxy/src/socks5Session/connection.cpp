#include "sshProxy/socks5Session.hpp"
#include "socks5Values/clientConnect.hpp"
#include "socks5Values/address.hpp"
#include "loggerMacro.hpp"

void sshProxy::socks5Session::connection(socks5Values::clientConnect &connection) {
  createLogger(logger);
  // Make sure that the SSH server is still on
  if (ssh_is_connected(session->getCSession()) == 0) {
    logger.warn("Ssh server is offline, not attempting to proxy traffic. Expect blocks");
    connectLocal(connection);
    // Some failure
    closeBoth();
    return;
  }
  // Test if site is blocked
  if (isBlocked(connection.destinationAddress.string())) {
    logger.debug("Destination blocked, proxying traffic");
    connectSsh(connection);
  } else {
    logger.debug("Destination not blocked, relaying");
    connectLocal(connection);
  }
}