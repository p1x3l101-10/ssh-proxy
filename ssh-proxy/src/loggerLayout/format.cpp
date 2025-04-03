#include "ssh-proxy.hpp"
#include <sstream>
#include <chrono>
#include <log4cpp/Priority.hh>
#include <string>

static auto program_start = std::chrono::system_clock::now();
using std::string;

std::string sshProxy::loggerLayout::format(const log4cpp::LoggingEvent& event) {
  std::ostringstream out;

  // Get log time
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = now-program_start;
  string elapsedStr = std::to_string(elapsed.count());

  // Padding
  elapsedStr.resize(10, ' ');
  string priority = "[" + log4cpp::Priority::getPriorityName(event.priority) + "]";
  priority.resize(7, ' ');

  // Create the output string
  out << elapsedStr << priority;
  if (! event.categoryName.empty()) {
    out << " (" << event.categoryName << ")";
  } else {
    out << " ";
  }
  out << ": " << event.message << "\n";

  return out.str();
}