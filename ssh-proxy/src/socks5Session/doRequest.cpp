#include "socks5Values/clientConnect.hpp"
#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"
#include <unistd.h>
#include <csignal>

void sshProxy::socks5Session::doRequest() {
  auto self(shared_from_this());
  auto header = std::make_shared<std::vector<uint8_t>>(7);
  boost::asio::async_read(socket, boost::asio::buffer(*header),
    [this, self, header](boost::system::error_code ec, std::size_t len) {
      createLogger(logger);
      if (ec) {
        logger.errorStream() << "Error reading request header: " << ec.message();
        return;
      }
      socks5Values::clientConnect request(*header);
      logger.debug("Recived socks5 request");
      
      openSshTunnel(request.destinationAddress, request.destinationPort);
    }
  );
}