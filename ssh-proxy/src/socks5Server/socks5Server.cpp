#include "sshProxy/configFile.hpp"
#include "sshProxy/socks5Server.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>

using boost::asio::ip::tcp;

sshProxy::socks5Server::socks5Server(boost::asio::io_context &ctx, std::shared_ptr<configFile> config, std::shared_ptr<ssh::Session> session)
: config(config)
, session(session)
, acceptor({
  ctx,
  tcp::endpoint(
      tcp::v4(),
    config->getConfig().clientPort
  )
}) {
  logger.debugStream() << "Created socks5 Server on port " << config->getConfig().clientPort;
  acceptConnection();
}