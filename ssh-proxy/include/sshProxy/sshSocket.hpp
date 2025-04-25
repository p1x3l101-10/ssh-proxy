#pragma once
#include "config.hpp"
#ifdef BUILD_WITH_SSH
#include <libssh/libsshpp.hpp>
#include <memory>
#include <boost/asio.hpp>
#include <mutex>
#include "asyncStream.hpp"

namespace sshProxy {
  class sshSocket : public asyncStream {
    private:
      boost::asio::any_io_executor executor;
      std::shared_ptr<ssh::Session> session;
      std::shared_ptr<ssh::Channel> channel;
      std::atomic<bool> isConnected;
      std::mutex mtx;
    public: // Parent methods
      using executor_type = boost::asio::any_io_executor;
      using shutdown_type = boost::asio::ip::tcp::socket::shutdown_type;
      explicit sshSocket(boost::asio::any_io_executor executor, std::shared_ptr<ssh::Session> sessionPtr);
      // Mimic boost sockets
      void async_read_some(
        boost::asio::mutable_buffer buffer,
        std::function<void(boost::system::error_code, std::size_t)> handler
      ) override;
      void async_write_some(
        boost::asio::const_buffer buffer,
        std::function<void(boost::system::error_code, std::size_t)> handler
      ) override;
      void async_connect (const boost::asio::ip::tcp::endpoint&, std::function<void(boost::system::error_code)> handler) override;
      void async_connect(
        const std::string address,
        const uint16_t portNum,
        std::function<void(boost::system::error_code)> handler
      ) override;
      void cancel() override {/* No-op */};
      void close(boost::system::error_code&) override { channel->close(); };
      void shutdown(shutdown_type what, boost::system::error_code& ec) override { channel->sendEof(); };
      executor_type get_executor() override { return executor; }
      bool is_open() const override { return channel->isOpen(); }
  };
}
#endif