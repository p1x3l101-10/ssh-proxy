cmake_minimum_required(VERSION 3.30)
project("ssh-proxy")
set(CMAKE_LIBS_OPTIONALS "cpm;boost")
set(CMAKE_LIBS_OPTIONALS_BOOST_VERSION "1.86.0")
set(CMAKE_LIBS_OPTIONALS_BOOST_COMPONENTS "process;asio")
include(cmake-libs/all)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD 17) # Library uses older standard and is not compatable with c++23????

CPMAddPackage(
    NAME tomlplusplus
    VERSION 3.4.0
    URL https://github.com/marzer/tomlplusplus/archive/refs/tags/v3.4.0.tar.gz
)

set(SSH_PROXY_DEFAULT_FAILURE -244)
build(
    BINARY ssh-proxy
    LIBRARIES tomlplusplus::tomlplusplus Boost::process
    INSTALL_TARGET
)
