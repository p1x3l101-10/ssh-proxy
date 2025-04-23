#include "sshProxy/sshSocket.hpp"
#include <thread>

void sshProxy::sshSocket::async_read_some(boost::asio::mutable_buffer buffer, std::function<void(boost::system::error_code, std::size_t)> handler) {
  std::thread([&isConnected = this->isConnected,channel = this->channel, executor = this->executor, buffer, handler = std::move(handler)](){
    // Sanity check
    if (!channel->isOpen()) {
      boost::asio::post(executor, [handler]() {
        handler(boost::asio::error::not_connected, 0);
      });
      return;
    }
    try {
      int bytes = channel->read(buffer.data(), buffer.size(), false);
      boost::asio::post(executor, [&channel, handler, bytes]() {
        if (bytes < 0) {
          // Error reading, return error code
          handler(boost::system::error_code(ssh_get_error_code(channel->getCChannel()), boost::system::generic_category()), 0);
        } else {
          // Successful read, pass size to handler
          handler(boost::system::error_code(), bytes);
        }
      });
    } catch (...) {
      boost::asio::post(executor, [handler]() {
        handler(boost::asio::error::operation_aborted, 0);
      });
    }
  }).detach();
}