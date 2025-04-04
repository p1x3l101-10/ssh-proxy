#pragma once
#include <string>
#include <log4cpp/Layout.hh>
#include <log4cpp/LoggingEvent.hh>

namespace sshProxy {
  class loggerLayout : public log4cpp::Layout {
    public:
      virtual ~loggerLayout() {};
      std::string format(const log4cpp::LoggingEvent& event);
  };
}