#ifndef SSH_PROXY_HPP
#define SSH_PROXY_HPP
#include <string>
#include <filesystem>
#include <vector>

namespace sshProxy {
    using std::string;
    using std::vector;
    using std::filesystem::path;
    class socksProxy {
        private:
            void serializeTOML(path toml);
            void generateCmd();
            char** convertC(vector<string> stringList);
            vector<string> execCmd;
            char** cmdArgv;
        protected:
            struct sshArgs {
                string username;
                string ipAddr;
                int port;
            };
            struct config {
                bool verbose;
                bool openAll;
                bool compress;
                int openPort;
                string sshBin;
            };
        public:
            sshArgs sshConf;
            config config;
            void execute();
            socksProxy(path toml) {
                serializeTOML(toml);
                generateCmd();
                cmdArgv = convertC(execCmd);
            };
    };
}

#endif