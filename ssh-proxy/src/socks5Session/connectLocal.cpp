#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"
#include "socks5Values/address.hpp"
#include "socks5Values/connectResponce.hpp"
#include "config.hpp"

using boost::asio::ip::tcp;

void sshProxy::socks5Session::connectLocal(socks5Values::clientConnect &connection) {
  auto self = shared_from_this();
  createLogger(logger);
  logger.debugStream() << "Connection diagram - IP: " << connection.destinationAddress.string() << ", Port: " << connection.destinationPort.string() << ", Address Type: " << magic_enum::enum_name(connection.destinationAddress.type);
  if (connection.destinationAddress.type != socks5Values::addressType::DOMAIN_NAME) {
    logger.debug("Skipping resolution");
    tcp::endpoint endpoint = {
      boost::asio::ip::make_address(connection.destinationAddress.string()),
      connection.destinationPort.portNum
    };
    // Start the connection timer
    connectTimer.expires_after(std::chrono::seconds(CONNECTION_TIMEOUT_SECONDS));
    connectTimer.async_wait([this, self](boost::system::error_code ec){
      createLogger(logger);
      if (!ec) {
        logger.error("Timed out");
        remoteSocket.cancel(); // Timed out
        socks5Values::connectResponce failure = socks5Values::responceStatus::TTL_EXPIRED;
        auto ret = async_write(clientSocket, boost::asio::buffer(failure.data()));
        closeBoth();
      } errorHandler
    });
    remoteSocket.async_connect(endpoint, [this, self, connection, endpoint](boost::system::error_code ec){
      createLogger(logger);
      if (!ec) {
        logger.debug("Created remote socket");
        // Acknowlage success
        socks5Values::connectResponce ack = {
          socks5Values::responceStatus::GRANTED,
          { socks5Values::addressType::IPV4, {0, 0, 0, 0} },
          0
        };
        async_write(clientSocket, boost::asio::buffer(ack.data()), [this, self](boost::system::error_code ec, std::size_t){
          connectTimer.cancel(); // Success
          createLogger(logger);
          if (!ec) {
            // Start connection
            logger.debug("Started data transfer");
            startClientToRemoteRelay();
            startRemoteToClientRelay();
          } errorHandler
        });
      } errorHandler
    });
  } else {
    logger.debug("Resolving endpoints");
    resolver.async_resolve(connection.destinationAddress.string(), connection.destinationPort.string(),
      [this, self, connection](boost::system::error_code ec, tcp::resolver::results_type results){
        createLogger(logger);
        if (!ec) {
          logger.debug("Resolved endpoints");
          connectTimer.expires_after(std::chrono::seconds(CONNECTION_TIMEOUT_SECONDS));
          connectTimer.async_wait([this, self](boost::system::error_code ec){
            createLogger(logger);
            if (!ec) {
              logger.error("Timed out");
              remoteSocket.cancel(); // Timed out
              socks5Values::connectResponce failure = socks5Values::responceStatus::TTL_EXPIRED;
              auto ret = async_write(clientSocket, boost::asio::buffer(failure.data()));
              closeBoth();
              } errorHandler
          });
          boost::asio::async_connect(remoteSocket, results, [this, self, connection](boost::system::error_code ec, const tcp::endpoint&){
            createLogger(logger);
            if (!ec) {
              logger.debug("Created remote socket");
              // Acknowlage success
              socks5Values::connectResponce ack = {
                socks5Values::responceStatus::GRANTED,
                { socks5Values::addressType::IPV4, {0, 0, 0, 0} },
                0
              };
              async_write(clientSocket, boost::asio::buffer(ack.data()), [this, self](boost::system::error_code ec, std::size_t){
                connectTimer.cancel(); // Success
                createLogger(logger);
                if (!ec) {
                  // Start connection
                  logger.debug("Started data transfer");
                  startClientToRemoteRelay();
                  startRemoteToClientRelay();
                } errorHandler
              });
            } errorHandler
          });
        } errorHandler
      }
    );
  }
}