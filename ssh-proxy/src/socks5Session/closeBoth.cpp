#include "sshProxy/socks5Session.hpp"

void sshProxy::socks5Session::closeBoth() {
  boost::system::error_code ignored;
  // I am aware that I should be getting the returns, i just am not using them
  clientSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored); //NOLINT
  clientSocket.close(ignored); //NOLINT
  remoteSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
  remoteSocket->close(ignored);
}
