#pragma once
#include <boost/asio/io_context.hpp>
#include "config.hpp"
#include <log4cpp/Category.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/LoggingEvent.hh>
#include <memory>
#include <boost/asio.hpp>
#include <libssh/libsshpp.hpp>
#include "sshProxy/configFile.hpp"

namespace sshProxy {
  class socks5Server {
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(NAME".socks5Server");
      std::shared_ptr<configFile> config;
      std::shared_ptr<ssh::Session> session;
      void acceptConnection();
      boost::asio::ip::tcp::acceptor acceptor;
    public:
      socks5Server(boost::asio::io_context &ioContext, std::shared_ptr<configFile> config, std::shared_ptr<ssh::Session> session);
  };
};