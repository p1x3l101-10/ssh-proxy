cmake_minimum_required(VERSION 3.30)
project("ssh-proxy")
set(CMAKE_LIBS_OPTIONALS "cpm")
include(cmake-libs/all)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD 17) # Library uses older standard and is not compatable with c++23????

CPMAddPackage(
    NAME tomlplusplus
    VERSION 3.4.0
    URL https://github.com/marzer/tomlplusplus/archive/refs/tags/v3.4.0.tar.gz
)

build(
    BINARY ssh-proxy
    LIBRARIES tomlplusplus::tomlplusplus
    INSTALL_TARGET
)
