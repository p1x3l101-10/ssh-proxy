#include "ssh-proxy.hpp"
#include "config.hpp"
#include <iostream>
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <memory>

int main(void) {
  // Set up logging
  log4cpp::Appender* appender = new log4cpp::OstreamAppender("console", &std::cout);
  appender->setLayout(new sshProxy::loggerLayout());
  log4cpp::Category& root = log4cpp::Category::getRoot();
  root.setAppender(appender);
  root.setPriority(log4cpp::Priority::DEBUG);
  root.debug("Logging initialized");
  log4cpp::Category& logger = log4cpp::Category::getInstance(CMAKE_PROJECT_NAME);
  logger.debug("Hierarchical application logging set up.");

  // Read the config
  logger.debug("Reading config file");
  std::shared_ptr<sshProxy::configFile> config(new sshProxy::configFile(CMAKE_INSTALL_SYSCONFDIR"/ssh-proxy.toml"));
  // End cleanup
  logger.debug("Reached end of execution");
  root.info("Goodbye");
  root.shutdown();
  return 0;
}