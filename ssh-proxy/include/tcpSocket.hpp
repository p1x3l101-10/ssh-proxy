#pragma once
#include "asyncStream.hpp"

class tcpSocket : public asyncStream {
  private:
    boost::asio::ip::tcp::socket internalSocket;
  public:
    explicit tcpSocket(boost::asio::ip::tcp::socket socket): internalSocket(std::move(socket)) {};
    void async_read_some (
      boost::asio::mutable_buffer buffer,
      std::function<void(boost::system::error_code, std::size_t)> handler
    ) override { internalSocket.async_read_some(buffer, std::move(handler)); }
    void async_write_some (
      boost::asio::const_buffer buffer,
      std::function<void(boost::system::error_code, std::size_t)> handler
    ) override { internalSocket.async_write_some(buffer, std::move(handler)); }
    void close() override {
      boost::system::error_code ignored;
      internalSocket.close(ignored); //NOLINT
    }
    void shutdown() override {
      boost::system::error_code ec;
      internalSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec); //NOLINT
    }
};