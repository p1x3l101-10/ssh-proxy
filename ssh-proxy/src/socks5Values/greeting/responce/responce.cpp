#include "socks5Values/greeting.hpp"

socks5Values::responce::responce(enum authTypes chosenAuth): version(0x05), chosenAuth(chosenAuth) {};