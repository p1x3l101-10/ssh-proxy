#include "socks5Values/port.hpp"

const std::array<uint8_t, 2> socks5Values::port::data() const {
  return {
    static_cast<uint8_t>((portNum >> 8) & 0xFF), // high byte
    static_cast<uint8_t>(portNum & 0xFF)         // low byte
  };
}