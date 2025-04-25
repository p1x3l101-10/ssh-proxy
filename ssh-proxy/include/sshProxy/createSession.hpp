#pragma once
#include "config.hpp"
#ifdef BUILD_WITH_SSH
#include <memory>
#include <libssh/libsshpp.hpp>
#include "sshProxy/configFile.hpp"

namespace sshProxy {
  std::shared_ptr<ssh::Session> createSession (std::shared_ptr<configFile> config);
}
#endif