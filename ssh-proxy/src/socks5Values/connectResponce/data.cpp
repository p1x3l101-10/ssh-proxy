#include "socks5Values/connectResponce.hpp"

const std::vector<uint8_t> socks5Values::connectResponce::data() const {
  std::vector<uint8_t> out = {
    version,
    static_cast<uint8_t>(status),
    reserved
  }; // Set up beginning of responce
  out.insert(out.end(), addr.data().begin(), addr.data().end()); // Append bindAddress
  out.insert(out.end(), bindPort.data().begin(), bindPort.data().end()); // Append bindPort (the array version)
  return out; // The format for the responce should be: { VER(1), STATUS(1), RES(1), BINDADDR(?), BINDPORT(2) }
}