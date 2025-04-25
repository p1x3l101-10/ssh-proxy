cmake_minimum_required(VERSION 3.30)
# Autoversion
execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX REPLACE "^v" "" GIT_TAG "${GIT_TAG}")
if(GIT_TAG)
    set(CMAKE_PROJECT_VERSION ${GIT_TAG})
else()
    set(CMAKE_PROJECT_VERSION "0.0.0")  # Fallback if no tag is found
endif()

project(
    "ssh-proxy"
    DESCRIPTION "A daemon that gets around blocking"
    HOMEPAGE_URL "https://github.com/p1x3l101-10/ssh-proxy"
    LANGUAGES CXX
    VERSION ${CMAKE_PROJECT_VERSION}
)
include(cmake-libs/all)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

option(BUILD_WITH_SSH "Build with ssh support (Disables core functions without)" ON)

find_package(Boost 1.87.0 REQUIRED COMPONENTS process headers system regex program_options)
find_package(magic_enum 0.9.7 CONFIG REQUIRED)
find_package(PkgConfig 2.4.3 REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
pkg_check_modules(log4cpp REQUIRED IMPORTED_TARGET log4cpp)

include(GNUInstallDirs)
if(PREFIXED_BUILD) # fix the build for things like homebrew
    set(CMAKE_INSTALL_FULL_SYSCONFDIR "${CMAKE_INSTALL_SYSCONFDIR}")
endif()

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
if(NOT BLOCK_TIMEOUT_SECONDS)
    set(BLOCK_TIMEOUT_SECONDS 3)
endif()
if(${BUILD_WITH_SSH})
    set(PROGRAM_NAME ${CMAKE_PROJECT_NAME})
else()
    set(PROGRAM_NAME "socks5Relay")
endif()
build(
    BINARY ssh-proxy
    LIBRARIES
        PkgConfig::tomlplusplus
        PkgConfig::log4cpp
        Boost::process
        Boost::system
        Boost::regex
        Boost::program_options
        magic_enum::magic_enum
    EMBED_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    INSTALL_TARGET
    PUBLIC_COMPILE_OPTIONS "-fexperimental-library"
)

if(${BUILD_WITH_SSH})
    find_package(libssh 0.11.1 REQUIRED)
    target_link_libraries(ssh-proxy PRIVATE ssh)
else()
    set_target_properties(ssh-proxy PROPERTIES OUTPUT_NAME "socks5Relay")
endif()

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" 
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROGRAM_NAME}"
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/ssh-proxy/sample.toml"
    RENAME "config.toml.example"
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/${PROGRAM_NAME}"
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/ssh-proxy/default.toml"
    RENAME "config.toml"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROGRAM_NAME}"
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
    RENAME "${PROGRAM_NAME}.8"
    TYPE MAN
)