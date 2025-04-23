#include "socks5Values/address.hpp"
#include <iostream>

const std::vector<uint8_t> socks5Values::address::data() const {
  std::vector<uint8_t> out; // Reserve size needed for vector
  out.reserve(1 + addr.size());
  out.push_back(static_cast<uint8_t>(type));
  out.insert(out.end(), addr.begin(), addr.end());
  return out;
}