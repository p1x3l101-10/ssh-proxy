#pragma once
#include <cstdint>
#include <vector>
#include "socks5Values/address.hpp"
#include "socks5Values/port.hpp"

namespace socks5Values {
  enum class responceStatus : uint8_t {
    GRANTED = 0x00,
    GENERAL_FAILURE = 0x01,
    CONNECTION_NOT_ALLOWED = 0x02,
    NETWORK_UNREACHABLE = 0x03,
    HOST_UNREACHABLE = 0x04,
    CONNECTION_REFUSED_BY_DEST = 0x05,
    TTL_EXPIRED = 0x06,
    PROTOCOL_ERROR = 0x07,
    ADDRESS_TYPE_NOT_SUPPORTED = 0x08
  };
  struct connectResponce {
    const uint8_t version = 0x05;
    enum responceStatus status;
    const uint8_t reserved = 0x00;
    const address addr;
    const port bindPort;
    connectResponce(enum responceStatus status, address addr, port bindPort)
    : status(status), addr(addr), bindPort(bindPort) {};
    const std::vector<uint8_t> data() const {
      std::vector<uint8_t> out = {
        version,
        static_cast<uint8_t>(status),
        reserved
      }; // Set up beginning of responce
      out.insert(out.end(), addr.data().begin(), addr.data().end()); // Append bindAddress
      out.insert(out.end(), bindPort.data().begin(), bindPort.data().end()); // Append bindPort (the array version)
      return out; // The format for the responce should be: { VER(1), STATUS(1), RES(1), BINDADDR(?), BINDPORT(2) }
    };
  };
}