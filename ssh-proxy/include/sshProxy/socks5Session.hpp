#pragma once
#include "socks5Values/greeting.hpp"
#include "socks5Values/address.hpp"
#include "socks5Values/port.hpp"
#include <boost/asio/io_context.hpp>
#include "config.hpp"
#include <log4cpp/Category.hh>
#include <memory>
#include <boost/asio.hpp>
#include <libssh/libsshpp.hpp>
#include "sshProxy/configFile.hpp"

namespace sshProxy {
  class socks5Session : public std::enable_shared_from_this<socks5Session> {
    private:
      std::array<enum socks5Values::authTypes, 1> supportedAuthMethods = { socks5Values::authTypes::NO_AUTH };
      socks5Values::authTypes selectAuthMethod(socks5Values::greeting greeting);
      log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".socks5Session");
      std::shared_ptr<configFile> config;
      boost::asio::ip::tcp::socket socket;
      std::shared_ptr<ssh::Session> session;
      std::shared_ptr<ssh::Channel> channel;
      void doHandShake();
      void doRequest();
      void openSshTunnel(socks5Values::address addr, socks5Values::port port);
      void startDataForwarding();
    public:
      socks5Session(boost::asio::ip::tcp::socket socket, std::shared_ptr<ssh::Session> session);
      void start() { doHandShake(); };
  };
}