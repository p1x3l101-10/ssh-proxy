#pragma once
#include <boost/asio.hpp>

class asyncStream {
  public:
    using executor_type = boost::asio::any_io_executor;
    using shutdown_type = boost::asio::ip::tcp::socket::shutdown_type;
    virtual ~asyncStream() = default;
    virtual executor_type get_executor() = 0;
    virtual void async_read_some(
      boost::asio::mutable_buffer buffer,
      std::function<void(boost::system::error_code,
      std::size_t)> handler
    ) = 0;
    virtual void async_write_some(
      boost::asio::const_buffer buffer,
      std::function<void(boost::system::error_code, std::size_t)> handler
    ) = 0;
    virtual void async_connect(
      const boost::asio::ip::tcp::endpoint& endpoint,
      std::function<void(boost::system::error_code)> handler
    ) = 0;
    virtual void async_connect(
      const std::string address,
      const uint16_t portNum,
      std::function<void(boost::system::error_code)> handler
    ) = 0;
    virtual void cancel() = 0;
    virtual void shutdown(shutdown_type what, boost::system::error_code& ec) = 0;
    virtual void close(boost::system::error_code& ec) = 0;
    virtual bool is_open() const = 0;
};