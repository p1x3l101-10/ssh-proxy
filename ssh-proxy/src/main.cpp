#include "ssh-proxy.hpp"
#include "config.hpp"
#include <string>
#include <filesystem>

using std::string;
using std::filesystem::path;

extern char** environ;

int main (int argc, char* argv[]) {
    path config = CMAKE_INSTALL_SYSCONFDIR"/ssh-proxy.toml";
    sshProxy::socksProxy* ssh = new sshProxy::socksProxy(config);
    ssh->execute();
    // Cleanup
    delete ssh;
    ssh = NULL;
    return 0;
}