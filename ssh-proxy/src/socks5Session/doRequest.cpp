#include "loggerMacro.hpp"
#include "ssh-proxy.hpp"
#include <unistd.h>
#include <csignal>

void sshProxy::socks5Session::doRequest() {
  auto self(shared_from_this());
  auto header = std::make_shared<std::vector<uint8_t>>(7); // Read VER, CMD, RSV, ATYP, DST.ADDR, DST.PORT // First byte is 0?????
  createLogger(earlyLogger);
  earlyLogger.debug("Starting request");
  boost::asio::async_read(socket, boost::asio::buffer(*header),
    [this, self, header](boost::system::error_code ec, std::size_t len) {
      createLogger(logger);
      if (ec) {
        logger.errorStream() << "Error reading request header: " << ec.message();
        return;
      }
      for (int i = 1; i < 5; ++i) { // Shave off first byte of header, appearantly the header starts from 1 and not 0
        (*header)[i-1] = (*header)[i];
      }
      len--;
      if (len != 6) {
        logger.errorStream() << "Incomplete SOCKS5 header, expected 6 bytes, got: " << len;
        return;
      }
      logger.debugStream() << "Read " << len << " bytes into header buffer";
      logger.debugStream() << "Received header: "
                   << "VER: " << static_cast<int>((*header)[0]) << ", "
                   << "CMD: " << static_cast<int>((*header)[1]) << ", "
                   << "RSV: " << static_cast<int>((*header)[2]) << ", "
                   << "ATYP: " << static_cast<int>((*header)[3]) << ", "
                   << "DST.ADDR: " << static_cast<int>((*header)[4]) << ", "
                   << "DST.PORT: " << static_cast<int>((*header)[5]);
      // Ensure we are using socks5
      uint8_t ver = (*header)[0];
      if (ver != 0x05) {
        logger.errorStream() << "Client sent malformed VER in header\n"
        << "\tWanted: " << std::hex << static_cast<int>(0x05) << std::dec << "\n"
        << "\tGot: " << std::hex << static_cast<int>(ver) << std::dec;
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
            if (ec || len != 1) return;
            uint8_t domainLen = (*addressPart)[0];
            auto domainAndPort = std::make_shared<std::vector<uint8_t>>(domainLen + 2); // +2 for port
            boost::asio::async_read(socket, boost::asio::buffer(*domainAndPort),
              [this, self, domainAndPort, header](boost::system::error_code ec, std::size_t len) {
                if (ec) return;
                createLogger(logger);
                // parse domain and port...
                // TODO: Actually impliment this...
                logger.critStream() << "You have reached a point of code that is not yet implimented\n"
                << "Make sure to not do whatever you did again.";
                if(kill(getpid(), SIGUSR2) == -1) {
                  logger.emerg("Unable to stop self, you are now on your own with a broken daemon...");
                  logger.emerg("Entering an infinite loop to prevent damage");
                  while (true) {}
                }
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