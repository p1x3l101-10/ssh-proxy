#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"

void sshProxy::socks5Session::startRemoteToClientRelay() {
  createLogger(logger);
  auto self(shared_from_this());
  auto buffer = std::make_shared<std::vector<uint8_t>>(4096);
  remoteSocket.async_read_some(boost::asio::buffer(*buffer), 
    [this, self, buffer](boost::system::error_code ec, std::size_t length){
      if (!ec) {
        async_write(clientSocket, boost::asio::buffer(buffer->data(), length),
          [this, self](boost::system::error_code ec, std::size_t length){
            if (!ec) {
              startRemoteToClientRelay();
            } errorHandlerSpec("write")
          }
        );
      } errorHandlerSpec("read")
    }
  );
}