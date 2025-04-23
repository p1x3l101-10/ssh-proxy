#pragma once
#include <libssh/libsshpp.hpp>
#include <memory>
#include <boost/asio.hpp>
#include "asyncStream.hpp"
#include "socks5Values/clientConnect.hpp"

namespace sshProxy {
  class sshSocket : public asyncStream {
    private:
      boost::asio::any_io_executor executor;
      std::shared_ptr<ssh::Session> session;
      std::shared_ptr<ssh::Channel> channel;
    public:
      explicit sshSocket(boost::asio::any_io_executor executor, std::shared_ptr<ssh::Session> sessionPtr, socks5Values::clientConnect &connection);
      // Mimic boost sockets
      void async_read_some(boost::asio::mutable_buffer buffer, const std::function<void(boost::system::error_code, std::size_t)>& handler);
      void async_write_some(boost::asio::const_buffer buffer, const std::function<void(boost::system::error_code, std::size_t)>& handler);
      void close() override { channel->close(); };
      void shutdown() override { channel->sendEof(); };
  };
}