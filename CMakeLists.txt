cmake_minimum_required(VERSION 3.30)
project("ssh-proxy")
include(cmake-libs/all)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

find_package(Boost REQUIRED COMPONENTS process)
find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

set(SSH_PROXY_DEFAULT_FAILURE -244)
build(
    BINARY ssh-proxy
    EMBED_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    LIBRARIES PkgConfig::tomlplusplus Boost::process
    INSTALL_TARGET
)
