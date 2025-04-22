#pragma once
#include <cstdint>
#include <sys/types.h>
#include <vector>
#include "socks5Values/address.hpp"
#include "socks5Values/port.hpp"

namespace socks5Values {
  enum class connectCommand : uint8_t {
    TCP_IP_STREAM_CONNECTION = 0x01,
    TCP_IP_PORT_BINDING = 0x02,
    ASSOSIATE_UDP_PORT = 0x03
  };
  class clientConnect {
    private:
      const address parseAddress(const std::vector<uint8_t> &data) const;
    public:
      const uint8_t version;
      const connectCommand command;
      const uint8_t reserved;
      const address destinationAddress;
      const port destinationPort;
      clientConnect(std::vector<uint8_t> data);
      const std::vector<uint8_t> data() const;
  };
}