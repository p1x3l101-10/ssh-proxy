#include "socks5Values/port.hpp"

const std::string socks5Values::port::string() const {
  return std::to_string(portNum);
}