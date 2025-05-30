#include "socks5Values/address.hpp"
#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <regex>
#include <string>
#include "config.hpp"
#include "socks5Values/clientConnect.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
boost::asio::thread_pool blockedThreadPool(4);

void sshProxy::socks5Session::isBlocked(const boost::asio::any_io_executor &ex, const socks5Values::clientConnect &connection, std::function<void(const socks5Values::clientConnect&,bool)> handler) {
  auto self = shared_from_this();
  boost::asio::post(ex, [this, self, &ex, connection = std::move(connection), handler = std::move(handler)](){
    try {
      boost::asio::post(blockedThreadPool,[this, self, &ex, connection = std::move(connection), handler = std::move(handler)](){
        std::function<void(bool)> sendBoost = [&ex, &connection, handler = std::move(handler)](bool isBlocked){
          boost::asio::post(ex, [connection, &isBlocked, handler = std::move(handler)](){
            handler(std::move(connection), isBlocked);
          });
        };
        createLogger(logger);
        auto host = connection.destinationAddress.string();
        auto port = connection.destinationPort.string();
        int portNum = static_cast<int>(connection.destinationPort.portNum);
        // Dont try raw IP
        if (connection.destinationAddress.type != socks5Values::addressType::DOMAIN_NAME) {
          sendBoost(false);
        }
        // Test for block
        if (portNum != 443 && portNum != 80) { // Not http, assume safe
          logger.debug("Not on standard http or https ports, assuming connection is safe");
          sendBoost(false);
          return;
        }
        try {
          // Start a connection
          net::io_context ioc;
          tcp::resolver resolver(ioc);
          beast::tcp_stream stream(ioc);

          // Start the timeout clock
          boost::asio::steady_timer connectTimer(ioc);
          connectTimer.expires_after(std::chrono::seconds(BLOCK_TIMEOUT));
          connectTimer.async_wait([this, self](boost::system::error_code ec){
            createLogger(logger);
            logger.warn("Timed out");
            throw;
          });

          // Resolve domain
          auto const results = resolver.resolve(host, "80");
          stream.connect(results);

          // Build HTTP GET request
          http::request<http::string_body> req{http::verb::get, "/", 11};
          req.set(http::field::host, host);
          req.set(http::field::user_agent, SPOOF_USERAGENT);

          // Send the request
          http::write(stream, req);

          // Read the response
          beast::flat_buffer buffer;
          http::response<http::string_body> res;
          http::read(stream, buffer, res);

          // Close the connection
          beast::error_code ec;
          stream.socket().shutdown(tcp::socket::shutdown_both, ec); //NOLINT
          connectTimer.cancel(); // We are done now

          // Look for LS categories
          std::string body = res.body();
          std::smatch match;
          std::regex titleRegex("<script type=\\'text/javascript\\'>(.*?)</script></head></html>", std::regex_constants::icase);
          if (std::regex_search(body, match, titleRegex)) {
            std::string script = match[1].str();
            logger.debugStream() << script;
            if (script.contains("location.replace('https://relay-proxy.norman.k12.ok.us:8443/redirect?x=")) { // Test for redirect
              sendBoost(true);
              return;
            }
          }
          sendBoost(false);
          return;
        } catch (...) {
          logger.warn("Could not detect if site is blocked, assuming worst case");
          sendBoost(true);
          return;
        }
      });
    } catch (...) {
      logger.error("Failed to start checker thread");
      closeBoth();
    };
  });
}