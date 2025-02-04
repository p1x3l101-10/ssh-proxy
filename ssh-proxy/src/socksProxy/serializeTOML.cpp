#include "ssh-proxy.hpp"
#include <filesystem>
#include <optional>
#include <toml++/toml.hpp>
#include <string>

using std::string;
using std::optional;
using std::filesystem::path;

void sshProxy::socksProxy::serializeTOML(path toml) {
    toml::table tbl = toml::parse_file(toml.string());
    socksProxy::sshConf.ipAddr = tbl["sshConf"]["ipAddr"].ref<string>();
    socksProxy::sshConf.username = tbl["sshConf"]["username"].ref<string>();
    optional<int> port = tbl["sshConf"]["port"].value<int>();
    socksProxy::sshConf.port = port.value_or(22);
    optional<string> keyFile = tbl["sshConf"]["keyFile"].value<string>();
    socksProxy::sshConf.keyFile = keyFile.value_or("UNSET");
    optional<int> openPort = tbl["config"]["openPort"].value<int>();
    socksProxy::config.openPort = openPort.value_or(1080);
    optional<bool> openAll = tbl["config"]["openAll"].value<bool>();
    socksProxy::config.openAll = openAll.value_or(false);
    optional<bool> compress = tbl["config"]["compress"].value<bool>();
    socksProxy::config.compress = compress.value_or(true);
    optional<string> sshBin = tbl["config"]["sshBin"].value<string>();
    socksProxy::config.sshBin = sshBin.value_or("/usr/bin/ssh");
    optional<bool> verbose = tbl["config"]["verbose"].value<bool>();
    socksProxy::config.verbose = verbose.value_or(false);
}