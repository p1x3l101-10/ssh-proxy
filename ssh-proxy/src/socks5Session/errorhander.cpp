#include "sshProxy/socks5Session.hpp"
#include "socks5Values/connectResponce.hpp"
#include <log4cpp/Category.hh>
#include <unistd.h>

namespace err = boost::asio::error;
using socks5Values::responceStatus;

void sshProxy::socks5Session::errorhander(boost::system::error_code &ec, const std::string loggerName) {
  // Create logger from other file, and signify this is in the errorhandler function
  log4cpp::Category& logger = log4cpp::Category::getInstance(loggerName + ".errorhander");
  // Figure out error from boost error code
  socks5Values::responceStatus code;
  switch (ec.value()) {
    case err::operation_aborted: return; // Stop telling me that the stupid timers were stopped
    case err::eof: {
      logger.debug("Connection terminated.");
      closeBoth();
      return;
    }
    case err::not_connected: {
      // Stop daemon when socket breaks
      logger.emerg("Socket failure!");
      closeBoth();
      exit(100);
    }
    case err::host_not_found:
    case err::host_unreachable: code = responceStatus::HOST_UNREACHABLE; break;
    case err::network_unreachable: code = responceStatus::NETWORK_UNREACHABLE; break;
    case err::connection_refused: code = responceStatus::CONNECTION_REFUSED_BY_DEST; break;
    case err::shut_down:
    case err::timed_out: code = responceStatus::TTL_EXPIRED; break;
    default: code = responceStatus::GENERAL_FAILURE; break;
  }
  logger.errorStream() << ec.message();
  socks5Values::connectResponce failure = code;
  auto ret = async_write(clientSocket, boost::asio::buffer(failure.data()));
  closeBoth();
}