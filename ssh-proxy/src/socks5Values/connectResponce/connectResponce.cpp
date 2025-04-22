#include "socks5Values/connectResponce.hpp"

// Normal constructor
socks5Values::connectResponce::connectResponce(enum responceStatus status, address addr, port bindPort)
: status(status), addr(addr), bindPort(bindPort) {};
// For failures
socks5Values::connectResponce::connectResponce(enum responceStatus error)
: status(error), addr({ socks5Values::addressType::IPV4, { 0, 0, 0, 0 } }), bindPort({ 0, 0 }) {};