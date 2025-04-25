#pragma once
#include "socks5Values/greeting.hpp"
#include <boost/asio/io_context.hpp>
#include "config.hpp"
#include <log4cpp/Category.hh>
#include <memory>
#include <boost/asio.hpp>
#ifdef BUILD_WITH_SSH
#include <libssh/libsshpp.hpp>
#endif
#include "socks5Values/clientConnect.hpp"
#include "sshProxy/configFile.hpp"
#include "asyncStream.hpp"

namespace sshProxy {
  class socks5Session : public std::enable_shared_from_this<socks5Session> {
    private:
      std::array<enum socks5Values::authTypes, 1> supportedAuthMethods = { socks5Values::authTypes::NO_AUTH };
      socks5Values::authTypes selectAuthMethod(socks5Values::greeting greeting);
      log4cpp::Category& logger = log4cpp::Category::getInstance(NAME".socks5Session");
      std::shared_ptr<configFile> config = nullptr;
      boost::asio::ip::tcp::socket clientSocket;
      std::shared_ptr<asyncStream> remoteSocket;
      boost::asio::ip::tcp::resolver resolver;
      boost::asio::steady_timer connectTimer;
      #ifdef BUILD_WITH_SSH
      std::shared_ptr<ssh::Session> session = nullptr;
      std::shared_ptr<ssh::Channel> channel = nullptr;
      #else
      std::shared_ptr<void> session = nullptr;
      std::shared_ptr<void> channel = nullptr;
      #endif
      std::atomic<bool> usingSsh{false};
      void closeBoth();
      void doHandShake();
      void doRequest();
      void connection(const socks5Values::clientConnect &connection);
      void isBlocked(const boost::asio::any_io_executor &ex, const socks5Values::clientConnect &connection, std::function<void(const socks5Values::clientConnect&,bool)> handler);
      void connectLocal(const socks5Values::clientConnect &connection);
      void connectSsh(const socks5Values::clientConnect &connection);
      void startClientToRemoteRelay();
      void startRemoteToClientRelay();
      void errorhander(boost::system::error_code &ec, const std::string loggerName);
    public:
      #ifdef BUILD_WITH_SSH
      socks5Session(boost::asio::ip::tcp::socket clientSocket, std::shared_ptr<ssh::Session> session);
      #else
      socks5Session(boost::asio::ip::tcp::socket clientSocket);
      #endif
      void start() { doHandShake(); };
  };
}