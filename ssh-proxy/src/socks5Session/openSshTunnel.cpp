#include "ssh-proxy.hpp"

void sshProxy::socks5Session::openSshTunnel(const std::string &host, uint16_t port) {
  createLogger(logger);
  auto self(shared_from_this());
  if (channel->openForward(host.c_str(), port, config->getConfig().clientAddr.c_str(), config->getConfig().clientPort) != SSH_OK) {
    logger.errorStream() << "Failed to open SSH tunnel: " << session->getError();
    return;
  }

  std::vector<uint8_t> successResponse = {SOCKS5_VERSION, 0, 0, SOCKS5_ADDR_IPV4, 0, 0, 0, 0, 0, 0};

  logger.info("Connection successful");
  async_write(socket, boost::asio::buffer(successResponse),
    [this,self](boost::system::error_code ec, std::size_t) {
      if (!ec) {
        startDataForwarding();
      }
    }
  );
}