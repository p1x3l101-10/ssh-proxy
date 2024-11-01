#include "ssh-proxy.hpp"
#include "config.hpp"
#include <iostream>
#include <string>
#include <filesystem>

using std::string;
using std::filesystem::path;

extern char** environ;

int main (int argc, char* argv[]) {
    bool debug = false;
    if ((string) argv[1] == "--dbg") {
        debug = true;
    }
    path config = CMAKE_INSTALL_SYSCONFDIR"/ssh-proxy.toml";
    if (debug) {
        std::cout << "Config file: " << config.string() << std::endl;
    }
    sshProxy::socksProxy* ssh = new sshProxy::socksProxy(config, debug);
    ssh->execute();
    // Cleanup
    delete ssh;
    ssh = NULL;
    return 0;
}