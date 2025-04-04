#include "ssh-proxy.hpp"
#include <memory>

sshProxy::socks5Session::socks5Session(tcp::socket socket, std::shared_ptr<ssh::Session> session)
: socket(std::move(socket))
, session(session)
, channel(std::make_shared<ssh::Channel>(*session))
{}