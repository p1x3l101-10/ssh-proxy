#include "sshProxy/sshSocket.hpp"

extern boost::asio::thread_pool sshSocketThreadPool;

void sshProxy::sshSocket::async_write_some(boost::asio::const_buffer buf, std::function<void(boost::system::error_code, std::size_t)> handler) {
  boost::asio::post(sshSocketThreadPool, [&isConnected = this->isConnected,channel = this->channel, executor = this->executor, buffer = std::string((const char*)buf.data(), buf.size()), handler = std::move(handler)](){
    std::function<void(boost::system::error_code,int)> sendBoost = [executor, handler = std::move(handler)](boost::system::error_code ec, int bytes){
      boost::asio::post(executor, [&ec, &bytes, handler = std::move(handler)](){
        handler(ec, bytes);
      });
    };
    // Sanity check
    if (!channel->isOpen()) {
      sendBoost(boost::asio::error::not_connected, 0);
      return;
    }
    try {
      int bytes = channel->write(buffer.data(), buffer.size(), false);
      if (bytes < 0) {
        // Error reading, return error code
        sendBoost(boost::system::error_code(ssh_get_error_code(channel->getCChannel()), boost::system::generic_category()), 0);
      } else {
        // Successful read, pass size to handler
        sendBoost(boost::system::error_code(), bytes);
      }
    } catch (...) {
      sendBoost(boost::asio::error::operation_aborted, 0);
    }
  });
}