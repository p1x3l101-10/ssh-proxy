#include "sshProxy/sshSocket.hpp"
#include <memory>

sshProxy::sshSocket::sshSocket(boost::asio::any_io_executor executor, std::shared_ptr<ssh::Session> sessionPtr, socks5Values::clientConnect &connection)
: executor(executor)
, session(sessionPtr)
, channel(std::make_shared<ssh::Channel>(*sessionPtr))
{
  channel->openForward(connection.destinationAddress.string().c_str(), connection.destinationPort.portNum, "127.0.0.1", 0);
}