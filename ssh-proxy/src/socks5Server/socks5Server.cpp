#include "ssh-proxy.hpp"
#include <boost/asio/io_context.hpp>

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
  logger.debug("Created socks5 Server");
  acceptConnection();
}