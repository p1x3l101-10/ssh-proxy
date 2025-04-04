#pragma once
#include <memory>
#include <libssh/libsshpp.hpp>
#include "sshProxy/configFile.hpp"

namespace sshProxy {
  std::shared_ptr<ssh::Session> createSession (std::shared_ptr<configFile> config);
}