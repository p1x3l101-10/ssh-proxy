#include "config.hpp"
#include "sshProxy/configFile.hpp"
#ifdef BUILD_WITH_SSH
#include "sshProxy/createSession.hpp"
#endif
#include "sshProxy/loggerLayout.hpp"
#include "sshProxy/socks5Server.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <memory>
#include <magic_enum/magic_enum.hpp>
#include <boost/program_options/variables_map.hpp>

enum class PriorityLevelMirror { // Magic enum can understand this
  EMERG  = 0, 
  FATAL  = 1,
  ALERT  = 10,
  CRIT   = 20,
  ERROR  = 30, 
  WARN   = 40,
  NOTICE = 50,
  INFO   = 60,
  DEBUG  = 70,
  NOTSET = 80
};

void argProcesser(std::pair<int,char**> args);
extern boost::program_options::variables_map args;
extern boost::asio::thread_pool blockedThreadPool;
#ifdef BUILD_WITH_SSH
extern boost::asio::thread_pool sshSocketThreadPool;
#endif
std::function<void()> emergencyShutdown;
std::function<void()> gracefulShutdown;

int main(int c, char** v) {
  // Process args
  argProcesser({c, v});
  // Set up logging
  log4cpp::Appender* appender;
  if (args.count("logfile")) {
    appender = new log4cpp::FileAppender("default", args["logfile"].as<std::string>());
  } else {
    appender = new log4cpp::OstreamAppender("console", &std::cout);
  }
  appender->setLayout(new sshProxy::loggerLayout());
  log4cpp::Category& root = log4cpp::Category::getRoot();
  root.setAppender(appender);
  if (args.count("loglevel")) { // Set loglevel
    auto loglevel = args["loglevel"].as<std::string>();
    auto newLog = magic_enum::enum_cast<PriorityLevelMirror>(loglevel);
    if (newLog.has_value()) {
      // Weird int math because magic_enum seems to not like values over 200 or so
      int log = static_cast<int>(newLog.value());
      if (log == 1) {
        log--;
      }
      log = log * 10;
      root.setPriority(static_cast<log4cpp::Priority::PriorityLevel>(log));
    } else {
      root.alertStream() << "Priority \"" << loglevel << "\" does not exist!";
      return 1;
    }
  } else {
    root.setPriority(DEFAULT_LOGLEVEL);
  }
  root.debug("Logging initialized");
  log4cpp::Category& logger = log4cpp::Category::getInstance(NAME".main");
  logger.debug("Hierarchical application logging set up.");

  if (args.count("daemon")) {
    // Start a context
    boost::asio::io_context ctx;
    auto workGuard = boost::asio::make_work_guard(ctx); // Keep context alive even when idling
  
    // Register signal handler into context
    logger.debug("Registering signal handlers");
    std::atomic<bool> doingGracefulShutdown = false;
    boost::asio::signal_set gracefulSignals(ctx, SIGINT, SIGTERM);
    gracefulShutdown = [&doingGracefulShutdown,&logger,&workGuard,&ctx](){
      // Something wants us to stop gracefully, so we shall
      boost::asio::steady_timer connectTimer(ctx);
      connectTimer.expires_after(std::chrono::seconds(5));
      connectTimer.async_wait([&logger](boost::system::error_code ec){
        logger.warn("Took too long to shut down");
        emergencyShutdown();
      });
      logger.info("Please wait, cleaning up...");
      blockedThreadPool.join();
      #ifdef BUILD_WITH_SSH
      sshSocketThreadPool.join();
      #endif
      connectTimer.cancel(); // Done waiting
      workGuard.reset();
      doingGracefulShutdown = true;
      ctx.stop();
    };
    gracefulSignals.async_wait([](auto, auto){gracefulShutdown();});
    boost::asio::signal_set ungracefulSignals(ctx, SIGUSR2);
    emergencyShutdown = [&logger,&workGuard,&ctx](){
      // Stop as an error
      logger.info("Freeing needed resources before emergency shutdown...");
      workGuard.reset();
      ctx.stop();
    };
    ungracefulSignals.async_wait([](auto, auto){emergencyShutdown();});
    // Read the config
    logger.debug("Reading config file");
    std::string configFile = DEFAULT_ADMIN_CONFIG_PATH;
    if (args.count("config")) {
      logger.debug("Loading config specified on commandline");
      configFile = args["config"].as<std::string>();
    }
    std::shared_ptr<sshProxy::configFile> config(new sshProxy::configFile(configFile));

    #ifdef BUILD_WITH_SSH
    auto session = sshProxy::createSession(config); // Start an ssh connection
    sshProxy::socks5Server server(ctx, config, session); // Start server
    #else
    sshProxy::socks5Server server(ctx, config); // Start server
    #endif

    logger.debug("Starting main loop");
    try {
      ctx.run();
    } catch (...) {
      emergencyShutdown();
      throw;
    }
    if (doingGracefulShutdown) {
      root.info("Goodbye");
      root.shutdown();
      return 0;
    } else {
      root.emerg("Main loop stopped unexpectedly, please refer to the logs to find out what happened");
      return -1;
    }
  }
}