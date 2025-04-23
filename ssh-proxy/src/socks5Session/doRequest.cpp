#include "socks5Values/address.hpp"
#include "socks5Values/clientConnect.hpp"
#include <cstdint>
#include <magic_enum/magic_enum.hpp>
#include "socks5Values/connectResponce.hpp"
#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"
#include <memory>
#include <stdexcept>
#include <vector>

void sshProxy::socks5Session::doRequest() {
  auto self(shared_from_this());
  auto header = std::make_shared<std::array<uint8_t, 4>>();
  boost::asio::async_read(clientSocket, boost::asio::buffer(*header),
    [this, self, header](boost::system::error_code ec, std::size_t len) {
      createLogger(logger);
      if (ec) {
        logger.errorStream() << "Error reading request header: " << ec.message();
        return;
      }
      // Read first 4 bytes to figure out length
      uint8_t atyp = header->at(3);
      std::size_t extra_size = 0;
      bool domain = false;
      switch (static_cast<enum socks5Values::addressType>(atyp)) {
        case socks5Values::addressType::IPV4: {
          extra_size = 4;
          break;
        }
        case socks5Values::addressType::IPV6: {
          extra_size = 16;
          break;
        }
        case socks5Values::addressType::DOMAIN_NAME: {
          extra_size = 1; // Need 1 byte for finding domain length
          domain = true;
          break;
        }
        default: {
          logger.debug("Client sent bad ATYP");
          socks5Values::connectResponce failure = socks5Values::responceStatus::ADDRESS_TYPE_NOT_SUPPORTED;
          auto ret = async_write(clientSocket, boost::asio::buffer(failure.data()));
          return;
        }
      }
      if (!domain) {
        extra_size = extra_size + 2; // Capture port, domain has special logic
      }
      logger.debug("Recived request header");
      // Read next chunk
      auto tailer = std::make_shared<std::vector<uint8_t>>(extra_size);
      async_read(clientSocket, boost::asio::buffer(*tailer),
        [self, this, header, tailer, domain](boost::system::error_code ec, std::size_t){
          createLogger(logger);
          if (domain) {
            logger.debug("Using domain");
            // Domain logic
            uint8_t len = tailer->at(0);
            std::size_t extra_size = len + 2;
            auto domain_buf = std::make_shared<std::vector<uint8_t>>(extra_size); // Capture port
            async_read(clientSocket, boost::asio::buffer(*domain_buf),
              [self, this, header, tailer, domain_buf](boost::system::error_code ec, std::size_t){
                createLogger(logger);
                std::vector<uint8_t> data;
                data.reserve(header->size() + tailer->size() + domain_buf->size());
                data.insert(data.end(), header->begin(), header->end());
                data.insert(data.end(), tailer->begin(), tailer->end());
                data.insert(data.end(), domain_buf->begin(), domain_buf->end());
                try {
                  socks5Values::clientConnect connect(data);
                  connection(connect);
                } catch (std::runtime_error &e) {
                  if (std::strcmp(e.what(), "Bad address size")) {
                    logger.debug("Client sent bad address");
                    socks5Values::connectResponce failure = socks5Values::responceStatus::PROTOCOL_ERROR;
                    auto ret = async_write(clientSocket, boost::asio::buffer(failure.data()));
                    return;
                  } else {
                    throw e; // Not my error
                  }
                }
              }
            );
          } else { // IP logic
            logger.debug("Using IP");
            std::vector<uint8_t> data;
            data.reserve(header->size() + tailer->size());
            data.insert(data.end(), header->begin(), header->end());
            data.insert(data.end(), tailer->begin(), tailer->end());
            socks5Values::clientConnect connect = [&]() -> socks5Values::clientConnect {
              try {
                return socks5Values::clientConnect(data);
              } catch (const std::runtime_error &e) {
                if (std::strcmp(e.what(), "Bad address size") == 0) {
                  logger.debug("Client sent bad address");
                  socks5Values::connectResponce failure = socks5Values::responceStatus::PROTOCOL_ERROR;
                  auto ret = async_write(clientSocket, boost::asio::buffer(failure.data()));
                  closeBoth(); // Shut down this controlpath
                } else {
                  throw; // Not my error
                }
              }
            }(); // NOLINT
            connection(connect);
          }
        }
      );
    }
  );
}