#include "sshProxy/socks5Session.hpp"

void sshProxy::socks5Session::closeBoth() {
  boost::system::error_code ignored;
  clientSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
  clientSocket.close(ignored);
  remoteSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
  remoteSocket.close(ignored);
}
