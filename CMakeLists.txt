cmake_minimum_required(VERSION 3.30)
project("ssh-proxy")
include(cmake-libs/all)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

find_package(Boost REQUIRED COMPONENTS process)
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
include(FetchContent)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

set(SSH_PROXY_DEFAULT_FAILURE -244)
build(
    BINARY ssh-proxy
    LIBRARIES tomlplusplus::tomlplusplus Boost::process
    INSTALL_TARGET
)
