#include "socks5Values/address.hpp"
#include <stdexcept>

void socks5Values::address::validate() {
  switch (type) {
    case socks5Values::addressType::IPV4: if (addr.size() != 4) throw std::runtime_error("Bad address size"); break;
    case socks5Values::addressType::IPV6: if (addr.size() != 16) throw std::runtime_error("Bad address size"); break;
    case socks5Values::addressType::DOMAIN_NAME: break;
  }
}