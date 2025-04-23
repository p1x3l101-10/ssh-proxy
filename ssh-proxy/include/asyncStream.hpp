#pragma once
#include <boost/asio.hpp>

class asyncStream {
  public:
    virtual ~asyncStream() = default;
    virtual void async_read_some(
      boost::asio::mutable_buffer buffer,
      std::function<void(boost::system::error_code,
      std::size_t)> handler
    ) = 0;
    virtual void async_write_some(
      boost::asio::const_buffer buffer,
      std::function<void(boost::system::error_code, std::size_t)> handler
    ) = 0;
    virtual void shutdown() = 0;
    virtual void close() = 0;
};