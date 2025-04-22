#include "socks5Values/greeting.hpp"

const std::array<uint8_t, 2> socks5Values::responce::data() const {
  return {version, static_cast<uint8_t>(chosenAuth)};
}