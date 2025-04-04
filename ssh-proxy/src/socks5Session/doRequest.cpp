#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"
#include <sstream>
#include <string>
#include <unistd.h>
#include <csignal>

void sshProxy::socks5Session::doRequest() {
  auto self(shared_from_this());
  auto header = std::make_shared<std::vector<uint8_t>>(7);
  createLogger(earlyLogger);
  earlyLogger.debug("Starting request");
  boost::asio::async_read(socket, boost::asio::buffer(*header),
    [this, self, header](boost::system::error_code ec, std::size_t len) {
      createLogger(logger);
      if (ec) {
        logger.errorStream() << "Error reading request header: " << ec.message();
        return;
      }
      socks5Greeting greet = processGreet(header);
      {
        std::stringstream prettyAuthMethods;
        for (auto it = greet.auth.begin(); it != greet.auth.end(); it++) {
          bool last = std::next(it) == greet.auth.end();
          prettyAuthMethods << std::hex << static_cast<int>(*it) << std::dec;
          if (!last) {
            prettyAuthMethods << ", ";
          }
        }
        logger.debugStream() << "Received greeting: "
                     << "VER: " << static_cast<int>(greet.ver) << ", "
                     << "NAUTH: " << greet.auth.size() << ", "
                     << "AUTH: " << prettyAuthMethods.str();
      }
      // Ensure we are using socks5
      if (greet.ver != 0x05) {
        logger.errorStream() << "Client sent malformed VER in header\n"
        << "\tWanted: " << std::hex << static_cast<int>(0x05) << std::dec << "\n"
        << "\tGot: " << std::hex << static_cast<int>(greet.ver) << std::dec;
        return;
      }

      uint8_t atyp = (*header)[3];
      std::size_t addrLen = 0;

      switch (atyp) {
        case 0x01: addrLen = 4; break; // IPv4
        case 0x03: addrLen = 1; break; // Domain name (first byte is length)
        case 0x04: addrLen = 16; break; // IPv6
        default:
          logger.errorStream() << "Unknown address type: " << std::hex << static_cast<int>(atyp) << std::dec;
          return;
      };
      auto addressPart = std::make_shared<std::vector<uint8_t>>();
      if (atyp == 0x03) {
        // Read just the length byte first
        addressPart->resize(1);
        boost::asio::async_read(socket, boost::asio::buffer(*addressPart),
          [this, self, addressPart, header](boost::system::error_code ec, std::size_t len) {
            createLogger(logger);
            if (ec || len != 1) {
              logger.error("Failed to read domain length");
              return;
            }
            uint8_t domainLen = (*addressPart)[0];
            logger.debugStream() << "Domain length: " << static_cast<int>(domainLen);
            auto domainAndPort = std::make_shared<std::vector<uint8_t>>(domainLen + 2); // +2 for port
            boost::asio::async_read(socket, boost::asio::buffer(*domainAndPort),
              [this, self, domainAndPort, header, domainLen](boost::system::error_code ec, std::size_t len) {
                createLogger(logger);
                if (ec || len != domainAndPort->size()) {
                  logger.error("Failed to read domain name and port");
                  return;
                }
                // Extract domain name from the buffer
                std::string domain(domainAndPort->begin(), domainAndPort->begin() + domainLen);
                uint16_t port = ( (*domainAndPort)[domainLen] << 8 ) | (*domainAndPort)[domainLen + 1];

                // Log
                logger.debugStream() << "Domain: '" << domain << "', Port: '" << port << "'";

                // Resolve the domain
                boost::asio::ip::tcp::resolver resolver(socket.get_executor());
                // Resolve the domain name and port directly using resolver::resolve
                resolver.async_resolve(domain, std::to_string(port),
                  [this, self, domainAndPort](boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
                    createLogger(logger);
                    if (ec) {
                      logger.errorStream() << "Failed to resolve domain: " << ec.message();
                      return;
                    }
                    for (const auto& endpoint : results) {
                      logger.debugStream() << "Resolved IP: " << endpoint.endpoint().address().to_string()
                                     << " Port: " << endpoint.endpoint().port();
                    }
                  }
                );
              }
            );
          }
        );
      } else {
        // IPv4 or IPv6: read addr + 2 bytes of port
        addressPart->resize(addrLen + 2);
        boost::asio::async_read(socket, boost::asio::buffer(*addressPart),
          [this, self, header, addressPart, addrLen](boost::system::error_code ec, std::size_t len) {
            createLogger(logger);
            if (ec) return;

            uint16_t port;
            std::memcpy(&port, &(*addressPart)[addrLen], 2);
            port = ntohs(port);

            std::string host;

            if ((*header)[3] == 0x01) { // IPv4
              char ipStr[INET_ADDRSTRLEN];
              std::memcpy(&ipStr, &(*addressPart)[0], 4);
              inet_ntop(AF_INET, &ipStr, ipStr, sizeof(ipStr));
              host = ipStr;
            } else if ((*header)[3] == 0x04) { // IPv6
              char ipStr[INET6_ADDRSTRLEN];
              std::memcpy(&ipStr, &(*addressPart)[0], 16);
              inet_ntop(AF_INET6, &ipStr, ipStr, sizeof(ipStr));
              host = ipStr;
            }

            logger.infoStream() << "Connection request sent to: socks5://" << host << ":" << port;
            openSshTunnel(host, port);
          }
        );
      }
    }
  );
}