#pragma once
#include "asyncStream.hpp"

class tcpSocket : public asyncStream {
  private:
    boost::asio::ip::tcp::socket internalSocket;
  public:
    using executor_type = boost::asio::any_io_executor;
    using shutdown_type = boost::asio::ip::tcp::socket::shutdown_type;
    explicit tcpSocket(boost::asio::ip::tcp::socket socket): internalSocket(std::move(socket)) {};
    void async_read_some (
      boost::asio::mutable_buffer buffer,
      std::function<void(boost::system::error_code, std::size_t)> handler
    ) override { internalSocket.async_read_some(buffer, std::move(handler)); }
    void async_write_some (
      boost::asio::const_buffer buffer,
      std::function<void(boost::system::error_code, std::size_t)> handler
    ) override { internalSocket.async_write_some(buffer, std::move(handler)); }
    void close(boost::system::error_code& ec) override {
      internalSocket.close(ec); //NOLINT
    }
    void shutdown(shutdown_type what, boost::system::error_code& ec) override {
      internalSocket.shutdown(what, ec); //NOLINT
    }
    void async_connect(
      const boost::asio::ip::tcp::endpoint& endpoint,
      std::function<void(boost::system::error_code)> handler
    ) override {
      internalSocket.async_connect(endpoint, std::move(handler));
    }
    void async_connect(
      const std::string address,
      const uint16_t portNum,
      std::function<void(boost::system::error_code)> handler
    ) override {
      boost::asio::ip::tcp::endpoint endpoint = {
        boost::asio::ip::make_address(address),
        portNum
      };
      async_connect(endpoint, std::move(handler));
    }
    void cancel() override {
      internalSocket.cancel();
    }
    executor_type get_executor() override {
      return internalSocket.get_executor();
    }
    bool is_open() const override {
      return internalSocket.is_open();
    };
};