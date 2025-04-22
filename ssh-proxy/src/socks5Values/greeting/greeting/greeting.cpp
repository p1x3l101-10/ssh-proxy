#include "socks5Values/greeting.hpp"

socks5Values::greeting::greeting(std::vector<uint8_t> data) {
  auto it = std::ranges::find(data, (uint8_t) 0x05);
  if (it == data.end()) { // No valid start
    incomplete = true;
    ver = 0;
    nauth = 0;
    return;
  }
  // Get values from iterator
  ver = *it; it++;
  nauth = *it; it++;
  for (; it != data.end(); it++) {
    if (magic_enum::enum_contains<authTypes>(static_cast<uint8_t>(*it))) { // Disregard if invalid (probably private use)
      auth.push_back(static_cast<authTypes>(*it));
    }
  }
}