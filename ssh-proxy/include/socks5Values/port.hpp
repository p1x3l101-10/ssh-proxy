#pragma once
#include <cstdint>
#include <array>
#include <string>

namespace socks5Values {
  struct port {
    const uint16_t portNum;
    port(uint16_t portNum);
    port(std::array<uint8_t, 2> splitPortNum);
    const std::array<uint8_t, 2> data() const;
    const std::string string() const;
  };
}