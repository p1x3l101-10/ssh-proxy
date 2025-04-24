#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <regex>
#include <string>
#include <nlohmann/json.hpp>
#include "config.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using nlohmann::json;

bool sshProxy::socks5Session::isBlocked(std::string host) {
  return TEST_PROXY;
  createLogger(logger);
  try {
    net::io_context ioc;
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    // Resolve domain
    auto const results = resolver.resolve(host, "80");
    stream.connect(results);

    // Build HTTP GET request
    http::request<http::string_body> req{http::verb::get, "/", 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, "ssh-proxy-check");

    // Send the request
    http::write(stream, req);

    // Read the response
    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    // Close the connection
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec); //NOLINT

    // Look for LS categories
    std::string body = res.body();
    std::smatch match;
    std::regex titleRegex("<textarea id='categories' class='hidden'>(.*?)</textarea>", std::regex_constants::icase);
    if (std::regex_search(body, match, titleRegex)) {
      std::string cats = match[1].str();
      if (json::accept(cats)) {
        // Valid json, parsing...
        json test(cats);
        // Test if matches what the lightspeed categories look like
        if (test.is_structured()) {
          if (test["67"].is_string()) { // "67" is a catigory number (when writing, it is "access-denied")
            return true;
          }
        }
      }
    }

    return false;
  } catch (...) {
    logger.warn("Could not detect if site is blocked, assuming worst case");
    return true;
  }
}