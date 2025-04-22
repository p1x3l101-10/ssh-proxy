#pragma once
#include <cstdint>
#include <array>
#include <string>

namespace socks5Values {
  struct port {
    const uint16_t portNum;
    port(uint16_t portNum)
    : portNum(portNum) {};
    port(std::array<uint8_t, 2> splitPortNum)
    : portNum((static_cast<uint16_t>(splitPortNum[0]) << 8) | splitPortNum[1]) {};
    const std::array<uint8_t, 2> data() const {
      return {
        static_cast<uint8_t>((portNum >> 8) & 0xFF), // high byte
        static_cast<uint8_t>(portNum & 0xFF)         // low byte
      };
    }
    const std::string string() const {
      return std::to_string(portNum);
    }
  };
}