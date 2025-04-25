#include "config.hpp"
#ifdef BUILD_WITH_SSH
#include "sshProxy/sshSocket.hpp"
#include "config.hpp"

extern boost::asio::thread_pool sshSocketThreadPool;

void sshProxy::sshSocket::async_read_some(boost::asio::mutable_buffer buffer, std::function<void(boost::system::error_code, std::size_t)> handler) {
  boost::asio::post(sshSocketThreadPool, [&isConnected = this->isConnected,channel = this->channel, executor = this->executor, buffer, handler = std::move(handler)](){
    int bytes = 0;
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
      if (!isConnected) { // Wait for first reply to verify connection
        try {
          auto start = std::chrono::steady_clock::now();
          while ((std::chrono::steady_clock::now() - start).count() <= CONNECTION_TIMEOUT_SECONDS) { // Start a loop to wait for the server to be ready
            int ret = channel->poll(false);
            if (ret > 0) {
              isConnected = true;
              break;
            } else if (ret < 0) {
              break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
          }
        } catch (ssh::SshException &ex) {
          if (ex.getCode() == EAGAIN) {
            isConnected = true;
          } else {
            throw ex;
          }
        }
      }
      int bytes = channel->read(buffer.data(), buffer.size(), false);
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
#endif