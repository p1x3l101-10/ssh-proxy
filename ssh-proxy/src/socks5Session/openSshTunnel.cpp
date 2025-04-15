#include "socks5Values/address.hpp"
#include "sshProxy/socks5Session.hpp"
#include "socks5Values/connectResponce.hpp"
#include "loggerMacro.hpp"

void sshProxy::socks5Session::openSshTunnel(socks5Values::address addr, socks5Values::port port) {
  createLogger(logger);
  auto self(shared_from_this());
  enum socks5Values::responceStatus status;
  if (channel->openForward(addr.string().c_str(), port.portNum, config->getConfig().clientAddr.c_str(), config->getConfig().clientPort) != SSH_OK) {
    logger.errorStream() << "Failed to open SSH tunnel: " << session->getError();
    status = socks5Values::responceStatus::GENERAL_FAILURE;
  } else {
    logger.info("Connection successful");
    status = socks5Values::responceStatus::GRANTED;
  }

  socks5Values::connectResponce responce(
    status,
    addr,
    port
  );

  async_write(socket, boost::asio::buffer(responce.data()),
    [this,self](boost::system::error_code ec, std::size_t) {
      if (!ec) {
        startDataForwarding();
      }
    }
  );
}