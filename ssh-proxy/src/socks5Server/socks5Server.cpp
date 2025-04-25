#include "config.hpp"
#include "sshProxy/configFile.hpp"
#include "sshProxy/socks5Server.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>

using boost::asio::ip::tcp;

#ifdef BUILD_WITH_SSH
sshProxy::socks5Server::socks5Server(boost::asio::io_context &ctx, std::shared_ptr<configFile> config, std::shared_ptr<ssh::Session> session)
#else
sshProxy::socks5Server::socks5Server(boost::asio::io_context &ctx, std::shared_ptr<configFile> config)
#endif
: config(config)
#ifdef BUILD_WITH_SSH
, session(session)
#endif
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