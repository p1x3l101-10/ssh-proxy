#include "sshProxy/socks5Session.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/io_context.hpp>
#include <memory>
#include <tcpSocket.hpp>

using boost::asio::ip::tcp;

sshProxy::socks5Session::socks5Session(tcp::socket clientSocket, std::shared_ptr<ssh::Session> session)
: clientSocket(std::move(clientSocket))
, remoteSocket(std::make_shared<tcpSocket>(clientSocket.get_executor()))
, resolver(tcp::resolver(clientSocket.get_executor()))
, session(session)
, connectTimer(clientSocket.get_executor())
{}