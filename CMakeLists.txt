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
find_package(nlohmann_json 3.12.0 REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
pkg_check_modules(log4cpp REQUIRED IMPORTED_TARGET log4cpp)

if(NOT CONNECTION_TIMEOUT_SECONDS)
    set(CONNECTION_TIMEOUT_SECONDS 10)
endif()
if(NOT DEFAULT_LOGLEVEL)
    if(("${CMAKE_BUILD_TYPE}" STREQUAL "Debug") OR ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo"))
        set(DEFAULT_LOGLEVEL "DEBUG")
    else()
        set(DEFAULT_LOGLEVEL "INFO")
    endif()
endif()
if(NOT TEST_PROXY)
    set(TEST_PROXY false)
endif()
if(NOT SPOOF_USERAGENT)
    set(SPOOF_USERAGENT "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:137.0) Gecko/20100101 Firefox/137.0")
endif()
build(
    BINARY ssh-proxy
    LIBRARIES
        PkgConfig::tomlplusplus
        PkgConfig::log4cpp
        ssh
        Boost::process
        Boost::system
        Boost::regex
        Boost::program_options
        magic_enum::magic_enum
        nlohmann_json::nlohmann_json
    EMBED_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    INSTALL_TARGET
    PUBLIC_COMPILE_OPTIONS "-fexperimental-library"
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" 
    TYPE DOC 
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/ssh-proxy/sample.toml"
    RENAME "config.toml.example"
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/${CMAKE_PROJECT_NAME}"
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/ssh-proxy/default.toml"
    RENAME "config.toml"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}"
)

# Mandoc, also generate from `--help`
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/help.out
  COMMAND ssh-proxy --help > help.out
  DEPENDS ssh-proxy
  COMMENT "Generating help output for man page"
)
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/.gen/${CMAKE_PROJECT_NAME}/man.8"
  COMMAND ${CMAKE_COMMAND} -E echo_append ""
  COMMAND ${CMAKE_COMMAND} -DHELP_FILE=${CMAKE_BINARY_DIR}/help.out
                          -DTEMPLATE=${CMAKE_CURRENT_BINARY_DIR}/.gen/${CMAKE_PROJECT_NAME}/man.8.template
                          -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/.gen/${CMAKE_PROJECT_NAME}/man.8
                          -P ${CMAKE_SOURCE_DIR}/cmake/generate_man.cmake
  DEPENDS ${CMAKE_BINARY_DIR}/help.out
  COMMENT "Generating man page"
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/man.8.in ${CMAKE_CURRENT_BINARY_DIR}/.gen/${CMAKE_PROJECT_NAME}/man.8.template @ONLY)
add_custom_target(generate_man ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/.gen/${CMAKE_PROJECT_NAME}/man.8"
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/.gen/${CMAKE_PROJECT_NAME}/man.8
    RENAME "${CMAKE_PROJECT_NAME}.8"
    TYPE MAN
)