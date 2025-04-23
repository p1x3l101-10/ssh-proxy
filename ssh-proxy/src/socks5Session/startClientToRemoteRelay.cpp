#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"

void sshProxy::socks5Session::startClientToRemoteRelay() {
  createLogger(logger);
  auto self(shared_from_this());
  auto buffer = std::make_shared<std::vector<uint8_t>>(4096);
  clientSocket.async_read_some(boost::asio::buffer(*buffer),
    [this, self, buffer](boost::system::error_code ec, std::size_t length){
      if (!ec) {
        async_write(remoteSocket, boost::asio::buffer(buffer->data(), length),
          [this, self](boost::system::error_code ec, std::size_t length){
            if (!ec) {
              startClientToRemoteRelay();
            } errorHandlerSpec("write")
          }
        );
      } errorHandlerSpec("read")
    }
  );
}