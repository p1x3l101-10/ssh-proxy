#pragma once
#include <cstdint>
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
    address(enum addressType type, std::vector<uint8_t> addr);
    const std::vector<uint8_t> data() const;
    const std::string string() const;
    private: 
      void validate();
  };
}