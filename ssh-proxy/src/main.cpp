#include "arguments.hpp"
#include "config.hpp"
#include "sshProxy/configFile.hpp"
#include "sshProxy/createSession.hpp"
#include "sshProxy/loggerLayout.hpp"
#include "sshProxy/socks5Server.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <memory>

int main(int argc, char** argv) {
  // Process args
  arguments::arguments args(argc,argv);
  // Set up logging
  log4cpp::Appender* appender;
  if (args.map().contains("logFile")) {
    appender = new log4cpp::FileAppender("default", args.map()["logFile"]);
  } else {
    appender = new log4cpp::OstreamAppender("console", &std::cout);
  }
  appender->setLayout(new sshProxy::loggerLayout());
  log4cpp::Category& root = log4cpp::Category::getRoot();
  root.setAppender(appender);
  root.setPriority(log4cpp::Priority::DEBUG);
  root.debug("Logging initialized");
  log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".main");
  logger.debug("Hierarchical application logging set up.");

  // Read the config
  logger.debug("Reading config file");
  std::string configFile = CMAKE_INSTALL_SYSCONFDIR"/ssh-proxy.toml";
  if (args.map().contains("config")) {
    logger.debug("Loading config specified on commandline");
    configFile = args.map()["config"];
  }
  std::shared_ptr<sshProxy::configFile> config(new sshProxy::configFile(configFile));

  // Start an ssh connection
  auto session = sshProxy::createSession(config);

  // Start the socks5 server
  boost::asio::io_context ctx;
  sshProxy::socks5Server server(ctx, config, session);
  auto workGuard = boost::asio::make_work_guard(ctx); // Keep context alive even when idling
  logger.debug("Registering signal handlers");
  // Register signal handler into context
  std::atomic<bool> gracefulShutdown = false;
  boost::asio::signal_set gracefulSignals(ctx, SIGINT, SIGTERM);
  gracefulSignals.async_wait(
    [&](auto, auto){
      // Something wants us to stop gracefully, so we shall
      logger.info("Please wait, cleaning up...");
      gracefulShutdown = true;
      workGuard.reset();
      ctx.stop();
    }
  );
  boost::asio::signal_set ungracefulSignals(ctx, SIGUSR2);
  ungracefulSignals.async_wait(
    [&](auto, auto) {
      // Stop as an error
      logger.info("Freeing needed resources before emergency shutdown...");
      workGuard.reset();
      ctx.stop();
    }
  );
  logger.debug("Starting main loop");
  ctx.run();
  if (gracefulShutdown) {
    root.info("Goodbye");
    root.shutdown();
    return 0;
  } else {
    root.emerg("Main loop stopped unexpectedly, please refer to the logs to find out what happened");
    return -1;
  }
}