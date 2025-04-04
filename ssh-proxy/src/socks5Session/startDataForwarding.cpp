#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"

using boost::asio::buffer;

void sshProxy::socks5Session::startDataForwarding() {
  createLogger(logger);
  auto self(shared_from_this());

  auto clientBuffer = std::make_shared<std::vector<uint8_t>>(1024);
  auto sshBuffer = std::make_shared<std::vector<uint8_t>>(1024);

  // Read from client and forward to SSH
  socket.async_read_some(buffer(*clientBuffer),
    [this,self,clientBuffer](boost::system::error_code ec, std::size_t length) {
      if (!ec) {
        channel->write(clientBuffer->data(), length);
        startDataForwarding();
      }
    }
  );

  // Read from SSH and forward to client
  int sshBytesRead = channel->read(sshBuffer->data(), sshBuffer->size(), false);
  if (sshBytesRead > 0) {
    async_write(socket, buffer(sshBuffer->data(), sshBytesRead),
      [this, self, sshBuffer](boost::system::error_code ec, std::size_t) {
        if (!ec) {
          startDataForwarding();
        }
      }
    );
  }
}