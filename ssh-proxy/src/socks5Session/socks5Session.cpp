#include "sshProxy/socks5Session.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <memory>

using boost::asio::ip::tcp;

sshProxy::socks5Session::socks5Session(tcp::socket socket, std::shared_ptr<ssh::Session> session)
: socket(std::move(socket))
, session(session)
, channel(std::make_shared<ssh::Channel>(*session))
{}