#include "config.hpp"
#include "license.h"
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
#include <magic_enum/magic_enum.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int ac, char** av) {
  // Process args
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "prints this help message")
    ("version", "print the version info")
    ("license", "print the full license info for this program")
    ("config", po::value<std::string>(), "path to config file")
    ("logfile", po::value<std::string>(), "path to log file")
    ("loglevel", po::value<std::string>(), "minimum loglevel to use")
  ;
  po::variables_map vm;
  try {
    po::store(po::parse_command_line(ac, av, desc), vm);
  } catch (po::unknown_option &uo) {
    std::cerr << uo.what() << "\n"
         << desc << std::endl;
    return 1;
  }
  // Arg options before logging
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }
  if (vm.count("version")) {
    std::cout << CMAKE_PROJECT_NAME << " (version: " << CMAKE_PROJECT_VERSION << ")\n"
              << CMAKE_PROJECT_DESCRIPTION << "\n"
              << CMAKE_PROJECT_HOMEPAGE_URL << "\n"
              << "\tCopyright 2025 Scott Blatt, SPDX short identifier: BSD-3-Clause" << std::endl;
    return 0;
  }
  if (vm.count("license")) {
    std::cout << CMAKE_PROJECT_NAME << " (version: " << CMAKE_PROJECT_VERSION << "):\n"
              << LICENSE_TEXT << std::endl;
  }
  // Set up logging
  log4cpp::Appender* appender;
  if (vm.count("logfile")) {
    appender = new log4cpp::FileAppender("default", vm["logFile"].as<std::string>());
  } else {
    appender = new log4cpp::OstreamAppender("console", &std::cout);
  }
  appender->setLayout(new sshProxy::loggerLayout());
  log4cpp::Category& root = log4cpp::Category::getRoot();
  root.setAppender(appender);
  if (vm.count("loglevel")) { // Set loglevel
    auto loglevel = magic_enum::enum_cast<log4cpp::Priority::PriorityLevel>(vm["loglevel"].as<std::string>());
    if (loglevel.has_value()) {
      root.setPriority(loglevel.value());
    } else {
      root.setPriority(DEFAULT_LOGLEVEL);
    }
  } else {
    root.setPriority(DEFAULT_LOGLEVEL);
  }
  root.debug("Logging initialized");
  log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME".main");
  logger.debug("Hierarchical application logging set up.");

  // Start a context
  boost::asio::io_context ctx;
  auto workGuard = boost::asio::make_work_guard(ctx); // Keep context alive even when idling

  // Register signal handler into context
  logger.debug("Registering signal handlers");
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
  // Read the config
  logger.debug("Reading config file");
  std::string configFile = CMAKE_INSTALL_SYSCONFDIR"/ssh-proxy.toml";
  if (vm.count("config")) {
    logger.debug("Loading config specified on commandline");
    configFile = vm["config"].as<std::string>();
  }
  std::shared_ptr<sshProxy::configFile> config(new sshProxy::configFile(configFile));

  auto session = sshProxy::createSession(config); // Start an ssh connection
  sshProxy::socks5Server server(ctx, config, session); // Start server

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