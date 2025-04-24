#pragma once
#include <string>
#include <log4cpp/Category.hh>
#include <filesystem>
#include "config.hpp"

namespace sshProxy {
  class configFile {
    protected:
      struct connectionStruct {
        std::string username;
        std::string ipAddr;
        int port;
        std::string keyFile;
      };
      struct configStruct {
        std::string clientAddr;
        int clientPort;
        bool openAll;
        bool compress;
      };
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(NAME".configFile");
      connectionStruct connection;
      configStruct config;
    public:
      // Write once, read forever!
      connectionStruct getConnection() { return this->connection; };
      configStruct getConfig() { return this->config; };
      configFile(std::filesystem::path configFile);
  };
}