#include "ssh-proxy.hpp"
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sys/_types/_pid_t.h>
#include <unistd.h>
#include <cerrno>

using std::runtime_error;
using std::endl;

void sshProxy::socksProxy::execute() {
    pid_t p;
    int stat;
    p = fork();
    if (p == 0) {
        int ret = execvp(socksProxy::config.sshBin.c_str(), socksProxy::cmdArgv);
    } else {
        wait(&stat);
        if (WIFEXITED(stat)) {
            if (WEXITSTATUS(stat) != 0) {
                string err = "Child process exited with code: " + std::to_string(WEXITSTATUS(stat));
                std::cerr << "Dumped commandline: " << endl << '\t';
                while (*socksProxy::cmdArgv) {
                    std::cerr << *socksProxy::cmdArgv++ << ' ';
                }
                std::cerr << endl;
                throw runtime_error(err.c_str());
            }
        } else {
            throw runtime_error("Failed to get return code of child process");
        }
    } 
}