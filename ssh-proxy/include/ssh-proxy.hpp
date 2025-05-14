#ifndef SSH_PROXY_HPP
#define SSH_PROXY_HPP
#include <string>
#include <filesystem>
#include <vector>
#include <optional>
using std::optional;
#include <toml++/impl/array.hpp>

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
            bool debug = false;
        protected:
            struct sshArgs {
                string username;
                string ipAddr;
                int port;
                string keyFile;
                toml::array extraArgs;
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
            socksProxy(path toml, bool debug = false) {
                socksProxy::debug = debug;
                serializeTOML(toml);
                generateCmd();
            };
    };
}

#endif