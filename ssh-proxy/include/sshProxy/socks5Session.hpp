#pragma once
#include "socks5Values/greeting.hpp"
#include <boost/asio/io_context.hpp>
#include "config.hpp"
#include <log4cpp/Category.hh>
#include <memory>
#include <boost/asio.hpp>
#include <libssh/libsshpp.hpp>
#include "socks5Values/clientConnect.hpp"
#include "sshProxy/configFile.hpp"

namespace sshProxy {
  class socks5Session : public std::enable_shared_from_this<socks5Session> {
    private:
      std::array<enum socks5Values::authTypes, 1> supportedAuthMethods = { socks5Values::authTypes::NO_AUTH };
      socks5Values::authTypes selectAuthMethod(socks5Values::greeting greeting);
      log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".socks5Session");
      std::shared_ptr<configFile> config = nullptr;
      boost::asio::ip::tcp::socket clientSocket;
      boost::asio::ip::tcp::socket remoteSocket;
      boost::asio::ip::tcp::resolver resolver;
      boost::asio::steady_timer connectTimer;
      std::shared_ptr<ssh::Session> session = nullptr;
      std::shared_ptr<ssh::Channel> channel = nullptr;
      bool usingSsh = false;
      void closeBoth();
      void doHandShake();
      void doRequest();
      void connection(socks5Values::clientConnect &connection);
      bool isBlocked(std::string url);
      void connectLocal(socks5Values::clientConnect &connection);
      void connectSsh(socks5Values::clientConnect &connection);
      void startClientToRemoteRelay();
      void startRemoteToClientRelay();
      void errorhander(boost::system::error_code &ec, const std::string loggerName);
    public:
      socks5Session(boost::asio::ip::tcp::socket clientSocket, std::shared_ptr<ssh::Session> session);
      void start() { doHandShake(); };
  };
}