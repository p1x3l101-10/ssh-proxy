#include "sshProxy/socks5Server.hpp"
#include "loggerMacro.hpp"
#include "sshProxy/socks5Session.hpp"
#include <boost/system/detail/error_code.hpp>

void sshProxy::socks5Server::acceptConnection() {
  createLogger(logger);
  logger.info("Started accepting connections");
  acceptor.async_accept(
    [this](boost::system::error_code ec, boost::asio::ip::tcp::socket clientSocket) {
      createLogger(logger);
      if (!ec) {
        std::make_shared<socks5Session>(
          std::move(clientSocket),
          session
        )->start();
      } else {
        logger.errorStream() << "Accept failed: " << ec.message();
      }
      acceptConnection();
    }
  );
}