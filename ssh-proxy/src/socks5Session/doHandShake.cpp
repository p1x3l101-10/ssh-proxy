#include "sshProxy/socks5Session.hpp"
#include "socks5Values/greeting.hpp"
#include "loggerMacro.hpp"
#include "config.hpp"
#include <boost/asio/read.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstring>
#include <memory>

void sshProxy::socks5Session::doHandShake() {
  auto self(shared_from_this());
  auto buffer = std::make_shared<std::vector<uint8_t>>(4);
  boost::asio::async_read(socket, boost::asio::buffer(*buffer),
    [this, self, buffer](boost::system::error_code ec, std::size_t length) {
      createLogger(logger);
      logger.debug("Performing handshake");
      std::shared_ptr<socks5Values::greeting> greeting;
      std::shared_ptr<socks5Values::responce> responce;
      if (buffer->size() < 4) {
        logger.debug("Did not get entire buffer");
        return;
      }
      greeting = std::make_shared<socks5Values::greeting>(*buffer);
      if (greeting->incomplete) {
        logger.debug("Incomplete greeting");
        return;
      }
      logger.debug("Deciding on auth method to use");
      responce = std::make_shared<socks5Values::responce>(selectAuthMethod(*greeting));
      if (!ec && greeting->ver != 0x05 && responce->chosenAuth != socks5Values::authTypes::DECLINE_AUTH) {
        logger.debug("Sending responce");
        async_write(socket, boost::asio::buffer(responce->data()),
          [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
              doRequest();
            }
          }
        );
      }
    }
  );
}