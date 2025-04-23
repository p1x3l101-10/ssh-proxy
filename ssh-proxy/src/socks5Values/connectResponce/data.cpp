#include "socks5Values/connectResponce.hpp"
#include <iostream>

const std::vector<uint8_t> socks5Values::connectResponce::data() const {
  std::vector<uint8_t> out = {
    version,
    static_cast<uint8_t>(status),
    reserved
  }; // Set up beginning of responce
  const auto addrData = addr.data();
  const auto portData = bindPort.data();
  out.insert(out.end(), addrData.begin(), addrData.end());
  out.insert(out.end(), portData.begin(), portData.end());
  return out; // The format for the responce should be: { VER(1), STATUS(1), RES(1), BINDADDR(?), BINDPORT(2) }
}