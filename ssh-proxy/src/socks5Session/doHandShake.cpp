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
  auto header = std::make_shared<std::vector<uint8_t>>(2);
  boost::asio::async_read(clientSocket, boost::asio::buffer(*header), [this, self, header](boost::system::error_code ec, std::size_t length) {
    createLogger(logger);
    logger.debug("Performing handshake");
    int nauth = header->at(1);
    auto auth = std::make_shared<std::vector<uint8_t>>(nauth);
    boost::asio::async_read(clientSocket, boost::asio::buffer(*auth), [this, self, header, auth](boost::system::error_code ec, std::size_t length) {
      createLogger(logger);
      std::vector<uint8_t> rawGreet;
      rawGreet.reserve(header->size() + auth->size());
      rawGreet.insert(rawGreet.end(), header->begin(), header->end());
      rawGreet.insert(rawGreet.end(), auth->begin(), auth->end());
      std::shared_ptr<socks5Values::greeting> greeting = std::make_shared<socks5Values::greeting>(rawGreet);
      std::shared_ptr<socks5Values::responce> responce;
      if (greeting->incomplete) {
        logger.debug("Incomplete greeting");
        return;
      }
      logger.debug("Deciding on auth method to use");
      responce = std::make_shared<socks5Values::responce>(selectAuthMethod(*greeting));
      if (!ec) {
        logger.debug("Sending responce");
        async_write(clientSocket, boost::asio::buffer(responce->data().data(), responce->data().size()),
          [this, self, responce](boost::system::error_code ec, std::size_t) {
            if (!ec) {
              doRequest();
            }
          }
        );
      }
    });
  });
}