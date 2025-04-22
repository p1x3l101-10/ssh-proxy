#pragma once
#include <array>
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
      const address parseAddress(const std::vector<uint8_t> &data) const {
        enum addressType atyp = static_cast<enum addressType>(data.at(3));
        switch (atyp) {
          using socks5Values::addressType;
          case addressType::IPV4: {
            return {atyp, std::vector<uint8_t>(data.begin() + 4, data.begin() + 8)};
          }
          case addressType::IPV6: {
            return {atyp, std::vector<uint8_t>(data.begin() + 4, data.begin() + 20)};
          }
          case addressType::DOMAIN_NAME: {
            uint8_t len = data.at(4);
            return {atyp, std::vector<uint8_t>(data.begin() + 5, data.begin() + 5 + len)};
          }
          default: {
            return {atyp, {}};
          }
        }
      };
    public:
      const uint8_t version;
      const connectCommand command;
      const uint8_t reserved;
      const address destinationAddress;
      const port destinationPort;
      clientConnect(std::vector<uint8_t> data)
      : version(data.at(0))
      , command(static_cast<enum connectCommand>(data.at(1)))
      , reserved(data.at(2))
      , destinationAddress(parseAddress(data))
      , destinationPort({data.end()[-2], data.back()}) {};
      const std::vector<uint8_t> data() const {
        std::vector<uint8_t> out = {
          version,
          static_cast<uint8_t>(command),
          reserved
        };
        out.insert(out.end(), destinationAddress.data().begin(), destinationAddress.data().end());
        out.insert(out.end(), destinationPort.data().begin(), destinationPort.data().end());
        return out;
      }
  };
}