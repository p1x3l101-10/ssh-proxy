#include "ssh-proxy.hpp"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;

void sshProxy::socksProxy::generateCmd() {
    vector<string> argv;
    argv.push_back(socksProxy::config.sshBin);
    {
        string groupArgs = "-";
        groupArgs.append("N"); // Noexec
        if (socksProxy::config.compress) {
            groupArgs.append("C");
        }
        if (socksProxy::config.verbose) {
            groupArgs.append("vvv");
        }
        if (groupArgs != "-") {
            argv.push_back(groupArgs);
        }
    }
    if (socksProxy::sshConf.port != 22) {
        argv.push_back("-p");
        argv.push_back(std::to_string(socksProxy::sshConf.port));
    }
    argv.push_back("-D");
    if (socksProxy::config.openAll) {
        argv.push_back("0.0.0.0:" + std::to_string(socksProxy::config.openPort));
    } else {
        argv.push_back(std::to_string(socksProxy::config.openPort));
    }
    if (socksProxy::sshConf.keyFile.compare("UNSET")) {
        argv.push_back("-i");
        argv.push_back(socksProxy::sshConf.keyFile);
    }
    {
        string sshAddr;
        sshAddr = socksProxy::sshConf.username + "@" + socksProxy::sshConf.ipAddr;
        argv.push_back(sshAddr);
    }
    if (socksProxy::debug) {
        std::cout << "Cmdline:" << '\t' << std::endl;
        for (const auto& arg : argv) {
            std::cout << arg << ' ';
        }
        std::cout << std::endl;
    }
    socksProxy::execCmd = argv;
}