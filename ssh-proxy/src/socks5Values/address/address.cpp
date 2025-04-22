#include "socks5Values/address.hpp"

socks5Values::address::address(enum addressType type, std::vector<uint8_t> addr)
: type(type), addr(std::move(addr)) { // Move to ensure minimum alloc
  validate(); // Ensure that sizes are correct for type
}