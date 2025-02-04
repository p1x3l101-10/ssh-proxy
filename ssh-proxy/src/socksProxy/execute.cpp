#include "ssh-proxy.hpp"
#include <iostream>
#include <stdexcept>
#include <boost/process/v2.hpp>
#include <boost/asio.hpp>
#include <string>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <sstream>

namespace bp2 = boost::process::v2;
namespace asio = boost::asio;
using std::string;
using std::filesystem::path;
using std::vector;
using std::stringstream;

path derefPath(string bin, string PATH) {
    // Parse PATH into a more usable format
    vector<string> path;
    {
        stringstream ss(PATH);
        string t;
        char del = ':';
        while(std::getline(ss, t, del)) {
            path.push_back(t);
        }
    }

    // Find the file
    class path binary;
    for (const string& test : path) {
        class path candidate = test + bin;
        if (std::filesystem::exists(candidate)) {
            binary = candidate;
            break;
        }
    }
    return binary;
}

void sshProxy::socksProxy::execute() {
    // Resolve the binary
    string binary;
    if (socksProxy::config.sshBin.c_str()[0] == '/') { // Absolute path
        binary = socksProxy::config.sshBin;
    } else if (
        ((socksProxy::config.sshBin.c_str()[0] == '.') && (socksProxy::config.sshBin.c_str()[1] == '/')) ||
        ((socksProxy::config.sshBin.c_str()[0] == '.') && (socksProxy::config.sshBin.c_str()[1] == '.') && (socksProxy::config.sshBin.c_str()[2] == '/'))
    ) { // Relitave path
        throw std::runtime_error("sshBin cannot be a relitave path");
    } else { // Dependant on $PATH
        binary = derefPath(socksProxy::config.sshBin, std::getenv("PATH")).string();
    }
    // Setup
    asio::io_context ctx;
    vector<string> args = socksProxy::execCmd;
    args.erase(args.begin());
    bp2::process proc(ctx, binary, args);

    // Exec
    int result = proc.wait();

    // If there are any failures in the child
    if ( result != 0 ) {
        std::cerr << "Dumping cmdline: " << std::endl;
        for (const std::string& substr : socksProxy::execCmd) {
            std::cerr << '\t' << substr << std::endl;
        }
        throw std::runtime_error("Process exited with code: " + std::to_string(result));
    }
}