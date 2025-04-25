#include "sshProxy/socks5Session.hpp"
#include "config.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/io_context.hpp>
#include <memory>
#include <tcpSocket.hpp>

using boost::asio::ip::tcp;
#ifdef BUILD_WITH_SSH
sshProxy::socks5Session::socks5Session(tcp::socket clientSocket, std::shared_ptr<ssh::Session> session)
#else
sshProxy::socks5Session::socks5Session(tcp::socket clientSocket)
#endif
: clientSocket(std::move(clientSocket))
, remoteSocket(std::make_shared<tcpSocket>(boost::asio::ip::tcp::socket(clientSocket.get_executor())))
, resolver(tcp::resolver(clientSocket.get_executor()))
#ifdef BUILD_WITH_SSH
, session(session)
#endif
, connectTimer(clientSocket.get_executor())
{}