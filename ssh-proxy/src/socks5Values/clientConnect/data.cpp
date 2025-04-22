#include "socks5Values/clientConnect.hpp"

const std::vector<uint8_t> socks5Values::clientConnect::data() const {
  std::vector<uint8_t> out = {
    version,
    static_cast<uint8_t>(command),
    reserved
  };
  out.insert(out.end(), destinationAddress.data().begin(), destinationAddress.data().end());
  out.insert(out.end(), destinationPort.data().begin(), destinationPort.data().end());
  return out;
}