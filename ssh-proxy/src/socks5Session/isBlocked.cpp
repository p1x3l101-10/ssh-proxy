#include "sshProxy/socks5Session.hpp"
#include "loggerMacro.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <regex>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

bool sshProxy::socks5Session::isBlocked(std::string host) {
  return true;
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

    // Look for title "Blocked"
    std::string body = res.body();
    std::smatch match;
    std::regex titleRegex("<title>(.*?)</title>", std::regex_constants::icase);
    if (std::regex_search(body, match, titleRegex)) {
      std::string title = match[1].str();
      std::transform(title.begin(), title.end(), title.begin(), ::tolower);
      return title.find("blocked") != std::string::npos;
    }

    return false;
  } catch (...) {
    // Could not connect or resolve — blocked for safety
    logger.warn("Could not detect if site is blocked, assuming worst case");
    return true;
  }
}