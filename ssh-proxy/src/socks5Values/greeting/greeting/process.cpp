#include "socks5Values/greeting.hpp"

std::vector<enum socks5Values::authTypes> socks5Values::greeting::process(std::vector<uint8_t> data) {
  if (data.size() - 2 != nauth) {
    incomplete = true;
    return {};
  }
  std::vector<enum authTypes> out;
  for (int i = 2; i < nauth; i++) {
    out.push_back(static_cast<enum authTypes>(data.at(i)));
  }
  return out;
}