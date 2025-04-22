#include "socks5Values/address.hpp"
#include <sstream>

const std::string socks5Values::address::string() const {
  std::stringstream out;
  switch (type) {
    case socks5Values::addressType::IPV4: {
      for (int i = addr.size() - 1; i >= 0; i--) {
        out << static_cast<int>(addr.at(i));
        if (i != 0) {
          out << ".";
        }
      }
      break;
    }
    case socks5Values::addressType::IPV6: {
      for (int i = addr.size() - 1; i >= 0; i = i - 2) {
        out << std::hex
            << static_cast<int>(addr.at(i + 1))
            << static_cast<int>(addr.at(i))
            << std::dec;
        if (i != 0) {
          out << "::";
        }
      }
      break;
    }
    case socks5Values::addressType::DOMAIN_NAME: {
      for (int i = 0; i < addr.size(); i++) {
        out << addr.at(i);
      }
      break;
    }
  }
  return out.str();
}