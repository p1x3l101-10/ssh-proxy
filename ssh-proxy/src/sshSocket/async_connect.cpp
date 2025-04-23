#include "sshProxy/sshSocket.hpp"
#include <cerrno>
#include <mutex>
#include <thread>

void sshProxy::sshSocket::async_connect(const boost::asio::ip::tcp::endpoint& endpoint, std::function<void(boost::system::error_code)> handler) {
  std::thread([&mtx = this->mtx, &isConnected = this->isConnected, channel = this->channel, executor = this->executor, handler = std::move(handler), endpoint]() mutable {
    std::scoped_lock lock(mtx);
    try {
      // Open a connection
      auto address = endpoint.address().to_string();
      auto port = endpoint.port();
      channel->openForward(address.c_str(), port, "127.0.0.1", 0);
      // Test for the connection to be on
      try {
        while (true) { // Start a loop to wait for the server to be ready
          int ret = channel->poll();
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
      if (isConnected) {
        boost::asio::post(executor, [handler = std::move(handler)]() {
          handler(boost::system::error_code{}); // success
        });
      } else {
        boost::asio::post(executor, [handler = std::move(handler)]() {
          handler(boost::system::error_code{boost::system::errc::io_error, boost::system::generic_category()});
        });
      }
    } catch (ssh::SshException& ex) {
      boost::asio::post(executor, [&ex, handler = std::move(handler)]() {
        handler(boost::system::error_code{ex.getCode(), boost::system::generic_category()});
      });
    }
  }).detach();
}

void sshProxy::sshSocket::async_connect(const std::string address, const uint16_t port, std::function<void(boost::system::error_code)> handler) {
  std::thread([&mtx = this->mtx, &isConnected = this->isConnected, channel = this->channel, executor = this->executor, handler = std::move(handler), address, port]() mutable {
    std::scoped_lock lock(mtx);
    try {
      // Open a connection
      channel->openForward(address.c_str(), port, "127.0.0.1", 0);
      // Test for the connection to be on
      try {
        while (true) { // Start a loop to wait for the server to be ready
          int ret = channel->poll();
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
      if (isConnected) {
        boost::asio::post(executor, [handler = std::move(handler)]() {
          handler(boost::system::error_code{}); // success
        });
      } else {
        boost::asio::post(executor, [handler = std::move(handler)]() {
          handler(boost::system::error_code{boost::system::errc::io_error, boost::system::generic_category()});
        });
      }
    } catch (ssh::SshException& ex) {
      boost::asio::post(executor, [&ex, handler = std::move(handler)]() {
        handler(boost::system::error_code{ex.getCode(), boost::system::generic_category()});
      });
    }
  }).detach();
}