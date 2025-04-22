#include "socks5Values/port.hpp"

socks5Values::port::port(uint16_t portNum)
: portNum(portNum) {};
socks5Values::port::port(std::array<uint8_t, 2> splitPortNum)
: portNum((static_cast<uint16_t>(splitPortNum[0]) << 8) | splitPortNum[1]) {};