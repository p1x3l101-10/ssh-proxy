#include <boost/asio.hpp>
#include <functional>
#include "asyncStream.hpp"

using boost::asio::ip::tcp;

void try_connect(std::shared_ptr<asyncStream> socket, tcp::resolver::results_type results, std::function<void(boost::system::error_code,const tcp::endpoint)> handler) {
  // A stand-in for the boost asio resolver, it does not like my polymorphic class
  auto iter = std::make_shared<tcp::resolver::results_type::iterator>(results.begin());
  auto end = results.end();

  std::function<void()> do_connect;
  do_connect = [=]() mutable {
    if (*iter == end) {
      handler(boost::asio::error::host_not_found, tcp::endpoint{});
      return;
    }
    auto endpoint = **iter;
    socket->async_connect(endpoint, [=](boost::system::error_code ec){
      if (!ec) {
        handler(ec, endpoint);
      } else {
        (*iter)++;
        do_connect();
      }
    });
  };
}