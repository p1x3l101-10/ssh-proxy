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
    const std::string string() {
      std::stringstream out;
      switch (type) {
        case socks5Values::addressType::IPV4: {
          for (int i = 0; i < 4; i++) {
            out << addr.at(i);
            if (i != 4) {
              out << ".";
            }
          }
        }
        case socks5Values::addressType::IPV6: {
          for (int i = 0; i < 16; i = i + 2) {
            out << addr.at(i);
            out << addr.at(i + 1);
            if (i != 16) {
              out << "::";
            }
          }
        }
        case socks5Values::addressType::DOMAIN_NAME: {
          int len = addr.at(0);
          for (int i = 1; i < len; i++) {
            out << addr.at(i);
          }
        }
      }
      return out.str();
    }
    private: 
      void validate() {
        switch (type) {
          case socks5Values::addressType::IPV4: if (addr.size() != 4) throw std::runtime_error("Bad address size"); break;
          case socks5Values::addressType::IPV6: if (addr.size() != 16) throw std::runtime_error("Bad address size"); break;
          case socks5Values::addressType::DOMAIN_NAME: {
            auto len = addr.at(0);
            if (addr.size() == 1) {
              throw std::runtime_error("Bad address format");
            }
            if (addr.size() - 1 != len) {
              throw std::runtime_error("Bad address size");
            }
          }
        }
      }
  };
}