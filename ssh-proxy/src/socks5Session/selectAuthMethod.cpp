#include "socks5Values/greeting.hpp"
#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"
#include <sstream>

socks5Values::authTypes sshProxy::socks5Session::selectAuthMethod(socks5Values::greeting greeting) {
  createLogger(logger);
  // List auth methods supported by client
  std::stringstream clientAuth;
  for (enum socks5Values::authTypes auth : greeting.auth) {
    clientAuth << magic_enum::enum_name(auth);
    if (auth != greeting.auth.back()) {
      clientAuth << ", ";
    }
  }
  logger.debugStream() << "Client supports auth methods: " << clientAuth.str();
  // Pick auth
  for (const auto& authCandidate : greeting.auth) {
    for (const auto& supportedAuthCandidate : supportedAuthMethods) {
      if (authCandidate == supportedAuthCandidate) {
        logger.debugStream() << "Successfully picked auth method: " << magic_enum::enum_name(authCandidate);
        return authCandidate;
      }
    }
  }
  logger.debug("Failed to agree on authentication method with client");
  return socks5Values::authTypes::DECLINE_AUTH;
}