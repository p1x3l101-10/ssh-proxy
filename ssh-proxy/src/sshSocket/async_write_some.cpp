#include "sshProxy/sshSocket.hpp"

void sshProxy::sshSocket::async_write_some(boost::asio::const_buffer buffer, const std::function<void(boost::system::error_code, std::size_t)>& handler) {
  boost::asio::post(executor, [this, buffer, handler]() {
    // Sanity check
    if (!channel->isOpen()) {
      handler(boost::asio::error::not_connected, 0);
      return;
    }
    // Perform the read over the SSH channel
    int bytesWritten = channel->write(buffer.data(), buffer.size(), false);
    if (bytesWritten < 0) {
      // Error reading, return error code
      handler(boost::system::error_code(ssh_get_error_code(channel->getCChannel()), boost::system::generic_category()), 0);
    } else {
      // Successful read, pass size to handler
      handler(boost::system::error_code(), bytesWritten);
    }
  });
}