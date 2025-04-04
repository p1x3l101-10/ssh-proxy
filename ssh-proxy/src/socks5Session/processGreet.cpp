#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"

const sshProxy::socks5Session::socks5Greeting sshProxy::socks5Session::processGreet(std::shared_ptr<std::vector<uint8_t>> header) {
  createLogger(logger);
  logger.debug("Processing greeting");
  socks5Greeting out;
  out.ver = header->at(0);
  int nauth = header->at(1);
  for (int i = 2; i < nauth + 2; i++) {
    out.auth.push_back(header->at(i));
  }
  out.auth.shrink_to_fit(); // No more things added to vector
  return out;
}