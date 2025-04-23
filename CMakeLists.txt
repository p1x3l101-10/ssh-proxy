cmake_minimum_required(VERSION 3.30)
project(
    "ssh-proxy"
    DESCRIPTION "A daemon that gets around blocking"
    HOMEPAGE_URL "https://github.com/p1x3l101-10/ssh-proxy"
    LANGUAGES CXX
    VERSION 2.0.0
)
include(cmake-libs/all)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

find_package(Boost 1.87.0 REQUIRED COMPONENTS process headers system regex program_options)
find_package(libssh 0.11.1 REQUIRED)
find_package(magic_enum 0.9.7 CONFIG REQUIRED)
find_package(PkgConfig 2.4.3 REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
pkg_check_modules(log4cpp REQUIRED IMPORTED_TARGET log4cpp)

set(CONNECTION_TIMEOUT_SECONDS 10)
set(DEFAULT_LOGLEVEL "DEBUG")
build(
    BINARY ssh-proxy
    LIBRARIES PkgConfig::tomlplusplus Boost::process ssh PkgConfig::log4cpp magic_enum::magic_enum Boost::system Boost::regex Boost::program_options
    INSTALL_TARGET
    PUBLIC_COMPILE_OPTIONS "-fexperimental-library"
)
