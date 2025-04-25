#include "sshProxy/socks5Server.hpp"
#include "loggerMacro.hpp"
#include "sshProxy/socks5Session.hpp"
#include "config.hpp"
#include <boost/system/detail/error_code.hpp>

void sshProxy::socks5Server::acceptConnection() {
  boost::asio::post(acceptor.get_executor(), [this](){
    createLogger(logger);
    acceptor.async_accept(
      [this](boost::system::error_code ec, boost::asio::ip::tcp::socket clientSocket) {
        createLogger(logger);
        if (!ec) {
          logger.info("Opened socket");
          std::make_shared<socks5Session>(
            std::move(clientSocket)
            #ifdef BUILD_WITH_SSH
            , session
            #endif
          )->start();
        } else {
          logger.errorStream() << "Accept failed: " << ec.message();
        }
        acceptConnection();
      }
    );
  });
}