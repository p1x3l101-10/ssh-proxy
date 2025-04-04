#pragma once
#include <cstdint>
#include <boost/asio/io_context.hpp>
#include "config.hpp"
#include <log4cpp/Category.hh>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <libssh/libsshpp.hpp>
#include "sshProxy/configFile.hpp"

namespace sshProxy {
  class socks5Session : public std::enable_shared_from_this<socks5Session> {
    protected:
      struct socks5Greeting {
        uint8_t ver;
        std::vector<uint8_t> auth; // Auth methods
      };
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".socks5Session");
      std::shared_ptr<configFile> config;
      boost::asio::ip::tcp::socket socket;
      std::shared_ptr<ssh::Session> session;
      std::shared_ptr<ssh::Channel> channel;
      const socks5Greeting processGreet(std::shared_ptr<std::vector<uint8_t>> header);
      void doHandShake();
      void doRequest();
      void openSshTunnel(const std::string &host, uint16_t port);
      void startDataForwarding();
    public:
      socks5Session(boost::asio::ip::tcp::socket socket, std::shared_ptr<ssh::Session> session);
      void start() { doHandShake(); };
  };
}