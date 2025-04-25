#pragma once
#include "config.hpp"
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
      #ifdef BUILD_WITH_SSH
      std::shared_ptr<ssh::Session> session;
      #endif
      void acceptConnection();
      boost::asio::ip::tcp::acceptor acceptor;
    public:
      #ifdef BUILD_WITH_SSH
      socks5Server(boost::asio::io_context &ioContext, std::shared_ptr<configFile> config, std::shared_ptr<ssh::Session> session);
      #else
      socks5Server(boost::asio::io_context &ioContext, std::shared_ptr<configFile> config);
      #endif
  };
};