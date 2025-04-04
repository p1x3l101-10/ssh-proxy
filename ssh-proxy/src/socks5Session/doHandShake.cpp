#include "ssh-proxy.hpp"
#include "config.hpp"
#include <boost/asio/read.hpp>
#include <boost/system/detail/error_code.hpp>

void sshProxy::socks5Session::doHandShake() {
  auto self(shared_from_this());
  auto buffer = std::make_shared<std::vector<uint8_t>>(2);
  boost::asio::async_read(socket, boost::asio::buffer(buffer->data(), 2),
    [this, self, buffer](boost::system::error_code ec, std::size_t length) {
      logger.debug("Performing handshake");
      if (!ec && length == 2 && (*buffer)[0] == SOCKS5_VERSION) {
        // Send handshake response (no authentication required)
        std::vector<uint8_t> response = {SOCKS5_VERSION, SOCKS5_NO_AUTH};
        async_write(socket, boost::asio::buffer(response),
          [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
              doRequest();
            }
          }
        );
      }
    }
  );
}