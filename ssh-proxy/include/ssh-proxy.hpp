#pragma once
#include <boost/asio/io_context.hpp>
#include <filesystem>
#include "config.hpp"
#include <log4cpp/Category.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/LoggingEvent.hh>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <libssh/libsshpp.hpp>

namespace sshProxy {
  using boost::asio::ip::tcp;
  using boost::asio::io_context;
  class loggerLayout : public log4cpp::Layout {
    public:
      virtual ~loggerLayout() {};
      std::string format(const log4cpp::LoggingEvent& event);
  };
  class configFile : public std::enable_shared_from_this<configFile> {
    protected:
      struct connectionStruct {
        std::string username;
        std::string ipAddr;
        int port;
        std::string keyFile;
      };
      struct configStruct {
        int clientPort;
        bool openAll;
        bool compress;
      };
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".configFile");
      connectionStruct connection;
      configStruct config;
    public:
      // Write once, read forever!
      connectionStruct getConnection() { return this->connection; };
      configStruct getConfig() { return this->config; };
      configFile(std::filesystem::path configFile);
  };
  std::shared_ptr<ssh::Session> createSession (std::shared_ptr<configFile> config);
  class socks5Session : public std::enable_shared_from_this<socks5Session> {
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".socks5Session");
      std::shared_ptr<configFile> config;
      io_context &ioContext;
      tcp::socket socket;
      ssh::Session &session;
      void doHandShake();
      void doRequest();
      void openSshTunnel();
      void startDataForwarding();
    public:
      socks5Session(std::shared_ptr<configFile> conf
                   , tcp::socket socket
                   , io_context &ioContext
                   , ssh::Session &session
                   ) : config(conf)
                     , socket(std::move(socket))
                     , ioContext(ioContext)
                     , session(session) {}
      void start() { doHandShake(); };
  };
  class socks5Server {
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".socks5Server");
      std::shared_ptr<configFile> config;
      std::shared_ptr<ssh::Session> session;
      void acceptConnection();
      tcp::acceptor acceptor;
    public:
      socks5Server(boost::asio::io_context &ioContext, std::shared_ptr<configFile> config, std::shared_ptr<ssh::Session> session);
  };
};