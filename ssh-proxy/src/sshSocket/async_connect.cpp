#include "sshProxy/sshSocket.hpp"
#include <cerrno>
#include <mutex>
#include <thread>

void sshProxy::sshSocket::async_connect(const boost::asio::ip::tcp::endpoint& endpoint, std::function<void(boost::system::error_code)> handler) {
  std::thread([&mtx = this->mtx, &isConnected = this->isConnected, channel = this->channel, executor = this->executor, handler = std::move(handler), endpoint]() mutable {
    std::scoped_lock lock(mtx);
    auto address = endpoint.address().to_string();
    auto port = endpoint.port();
    try {
      // Open a connection
      channel->openForward(address.c_str(), port, "127.0.0.1", 0);
      // Test for the connection to be on
      boost::asio::post(executor, [handler = std::move(handler)]() {
        handler(boost::system::error_code{}); // success
      });
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
      boost::asio::post(executor, [handler = std::move(handler)]() {
        handler(boost::system::error_code{}); // success
      });
    } catch (ssh::SshException& ex) {
      boost::asio::post(executor, [&ex, handler = std::move(handler)]() {
        handler(boost::system::error_code{ex.getCode(), boost::system::generic_category()});
      });
    }
  }).detach();
}