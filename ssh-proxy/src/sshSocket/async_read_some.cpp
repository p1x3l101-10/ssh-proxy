#include "sshProxy/sshSocket.hpp"
#include <thread>
#include "config.hpp"

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
      if (!isConnected) {
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