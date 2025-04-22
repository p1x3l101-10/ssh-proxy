#include "socks5Values/address.hpp"
#include "socks5Values/clientConnect.hpp"

using socks5Values::addressType;

const socks5Values::address socks5Values::clientConnect::parseAddress(const std::vector<uint8_t> &data) const {
  enum addressType atyp = static_cast<enum addressType>(data.at(3));
  switch (atyp) {
    case addressType::IPV4: {
      return {atyp, std::vector<uint8_t>(data.begin() + 4, data.begin() + 8)};
    }
    case addressType::IPV6: {
      return {atyp, std::vector<uint8_t>(data.begin() + 4, data.begin() + 20)};
    }
    case addressType::DOMAIN_NAME: {
      uint8_t len = data.at(4);
      return {atyp, std::vector<uint8_t>(data.begin() + 5, data.begin() + 5 + len)};
    }
    default: {
      return {atyp, {}};
    }
  }
}