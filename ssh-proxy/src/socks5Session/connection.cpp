#include "sshProxy/socks5Session.hpp"
#include "socks5Values/clientConnect.hpp"
#include "loggerMacro.hpp"
#include "config.hpp"
#ifdef BUILD_WITH_SSH
#include "sshProxy/createSession.hpp"
#endif

extern bool sshReachable;

void sshProxy::socks5Session::connection(const socks5Values::clientConnect &connection) {
  createLogger(logger);
  #ifdef BUILD_WITH_SSH
  // Make sure that the SSH server is still on
  if (!sshReachable || ssh_is_connected(session->getCSession()) == 0) {
    // Try to reconnect
    logger.info("Not connected to server, attempting to reconnect.");
    session = createSession(config);
    if (!sshReachable || ssh_is_connected(session->getCSession()) == 0) {
      logger.warn("Unable to connect to server, not attempting to proxy traffic. Expect blocks");
      connectLocal(connection);
      // Some failure
      closeBoth();
      return;
    }
  }
  // Test if site is blocked
  auto self = shared_from_this();
  isBlocked(clientSocket.get_executor(), connection, [this, self](const socks5Values::clientConnect &connection, bool isBlocked){
    createLogger(logger);
    if (isBlocked) {
      logger.debug("Destination blocked, proxying traffic");
      connectSsh(connection);
    } else {
      logger.debug("Destination not blocked, relaying");
      connectLocal(connection);
    }
  });
  #else
  logger.debug("SSH Disabled, defaulting to forwarding.");
  connectLocal(connection);
  #endif
}