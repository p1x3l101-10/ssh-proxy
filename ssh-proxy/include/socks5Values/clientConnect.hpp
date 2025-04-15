#pragma once
#include <array>
#include <cstdint>
#include <sys/types.h>
#include <vector>
#include "socks5Values/address.hpp"
#include "socks5Values/port.hpp"

namespace socks5Values {
  struct clientConnect {
    const uint8_t version;
    const uint8_t command;
    const uint8_t reserved;
    const address destinationAddress;
    const port destinationPort;
    clientConnect(std::vector<uint8_t> data)
    : version(data[0])
    , command(data[1])
    , reserved(data[2])
    , destinationAddress(static_cast<enum addressType>(data[3]), std::vector<uint8_t>(data.begin() + 4, data.end() - 2))
    , destinationPort({data[data.size() - 2], data[data.size() - 1]}) {};
    const std::vector<uint8_t> data() const {
      std::vector<uint8_t> out = {
        version,
        command,
        reserved
      };
      out.insert(out.end(), destinationAddress.data().begin(), destinationAddress.data().end());
      out.insert(out.end(), destinationPort.data().begin(), destinationPort.data().end());
      return out;
    }
  };
}