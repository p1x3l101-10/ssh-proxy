#include "socks5Values/clientConnect.hpp"

socks5Values::clientConnect::clientConnect(std::vector<uint8_t> data)
: version(data.at(0))
, command(static_cast<enum connectCommand>(data.at(1)))
, reserved(data.at(2))
, destinationAddress(parseAddress(data))
, destinationPort({data.end()[-2], data.back()}) {};