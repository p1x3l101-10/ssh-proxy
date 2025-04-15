#include "socks5Values/greeting.hpp"
#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"
#include <sstream>

socks5Values::authTypes sshProxy::socks5Session::selectAuthMethod(socks5Values::greeting greeting) {
  createLogger(logger);
  logger.debug("Selecting authentication method");
  for (const auto& authCandidate : greeting.auth) {
    for (const auto& supportedAuthCandidate : supportedAuthMethods) {
      if (authCandidate == supportedAuthCandidate) {
        logger.debug("Successfully picked auth method");
        return authCandidate;
      }
    }
  }
  logger.debug("Failed to agree on authentication method with client");
  return socks5Values::authTypes::DECLINE_AUTH;
}