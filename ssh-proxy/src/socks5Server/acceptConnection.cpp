#include "ssh-proxy.hpp"
#include <boost/system/detail/error_code.hpp>

void sshProxy::socks5Server::acceptConnection() {
  logger.debug("Started accepting connections");
  acceptor.async_accept(
    [this](boost::system::error_code ec, tcp::socket socket) {
      if (!ec) {
        std::make_shared<socks5Session>(
          std::move(socket),
          session
        )->start();
      }
    }
  );
}