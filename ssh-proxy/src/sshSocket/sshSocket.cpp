#include "sshProxy/sshSocket.hpp"
#include <memory>

sshProxy::sshSocket::sshSocket(boost::asio::any_io_executor executor, std::shared_ptr<ssh::Session> sessionPtr)
: executor(executor)
, session(sessionPtr)
, channel(std::make_shared<ssh::Channel>(*sessionPtr))
, isConnected(false)
{}