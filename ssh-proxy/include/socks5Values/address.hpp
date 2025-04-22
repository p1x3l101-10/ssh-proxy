#pragma once
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace socks5Values {
  enum class addressType : uint8_t {
    IPV4 = 0x01,
    DOMAIN_NAME = 0x03,
    IPV6 = 0x04
  };
  struct address {
    const addressType type;
    const std::vector<uint8_t> addr;
    address(enum addressType type, std::vector<uint8_t> addr)
    : type(type), addr(std::move(addr)) { // Move to ensure minimum alloc
      validate(); // Ensure that sizes are correct for type
    }
    const std::vector<uint8_t> data() const {
      std::vector<uint8_t> out(1 + addr.size()); // Reserve size needed for vector
      out.reserve(1 + addr.size());
      out.push_back(static_cast<uint8_t>(type));
      out.insert(out.end(), addr.begin(), addr.end());
      return out;
    };
    const std::string string() const {
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
    private: 
      void validate() {
        switch (type) {
          case socks5Values::addressType::IPV4: if (addr.size() != 4) throw std::runtime_error("Bad address size"); break;
          case socks5Values::addressType::IPV6: if (addr.size() != 16) throw std::runtime_error("Bad address size"); break;
          case socks5Values::addressType::DOMAIN_NAME: break;
        }
      }
  };
}