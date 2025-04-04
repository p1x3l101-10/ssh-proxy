#include "ssh-proxy.hpp"

void sshProxy::socks5Session::doRequest() {
  auto self(shared_from_this());
  auto buffer = std::make_shared<std::vector<uint8_t>>(2);
  createLogger(earlyLogger);
  earlyLogger.debug("Starting request");
  boost::asio::async_read(socket, boost::asio::buffer(buffer->data(), 2),
    [this, self, buffer](boost::system::error_code ec, std::size_t length) {
      if (!ec && length == 2 && (*buffer)[1] == SOCKS5_CMD_CONNECT && (*buffer)[3] == SOCKS5_ADDR_IPV4) {
        createLogger(logger);
        uint32_t ip;
        uint16_t port;
        std::memcpy(&ip, &(*buffer)[4], 4);
        std::memcpy(&port, &(*buffer)[8], 2);
        port = ntohs(port);

        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip, ipStr, sizeof(ipStr));
        logger.infoStream() << "Connection request sent to: socks5://" << ipStr << ":" << "port";

        openSshTunnel(ipStr, port);
      }
    }
  );
}