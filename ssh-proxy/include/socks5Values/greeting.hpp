#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <algorithm>
#include <magic_enum/magic_enum.hpp>

namespace socks5Values {
  enum class authTypes : uint8_t {
    NO_AUTH = 0x00,
    GSSAPI = 0x01, // RFC 1961
    USERNAME_PASSWORD = 0x02, // RFC 1929
    // Reserved by IINA (0x03 - 0x7F)
    CHALLENGE_HANDSHAKE_PROTOCOL = 0x03,
    // UNASSIGNED (0x04)
    CHALLENGE_RESPONCE_AUTH = 0x05,
    SECURE_SOCKETS_LAYER = 0x06,
    NDS_AUTH = 0x07, 
    MULTI_AUTH_FRAMEWORK = 0x08,
    JSON_PARAMATER_BLOCK = 0x09,
    // UNASSIGNED (0x0A - 0x7F)
    // Reserved for private use (0x80 - 0xFE)
    DECLINE_AUTH = 0xFF // Decline auth method
  };
  struct responce {
    responce(enum authTypes chosenAuth): version(0x05), chosenAuth(chosenAuth) {};
    const uint8_t version;
    const authTypes chosenAuth;
    const std::array<uint8_t, 2> data() const {
      return {version, static_cast<uint8_t>(chosenAuth)};
    };
  };
  class greeting {
    public: bool incomplete;
    private:
      std::vector<enum authTypes> process(std::vector<uint8_t> data) {
        if (data.size() + 2 != nauth) {
          incomplete = true;
          return {};
        }
        std::vector<enum authTypes> out;
        for (int i = 2; i < nauth; i++) {
          out.push_back(static_cast<enum authTypes>(data.at(i)));
        }
        return out;
      }
    public:
      uint8_t ver;
      uint8_t nauth;
      std::vector<enum authTypes> auth;
      greeting(std::vector<uint8_t> data) {
        auto it = std::ranges::find(data, (uint8_t) 0x05);
        if (it == data.end()) { // No valid start
          incomplete = true;
          ver = 0;
          nauth = 0;
          return;
        }
        // Get values from iterator
        ver = *it; it++;
        nauth = *it; it++;
        for (; it != data.end(); it++) {
          if (magic_enum::enum_contains<authTypes>(static_cast<uint8_t>(*it))) { // Disregard if invalid (probably private use)
            auth.push_back(static_cast<authTypes>(*it));
          }
        }
      };
  };
}