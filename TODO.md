# What needs to happen before release

- [ ] Get a working socks server
  - [ ] Relay traffic that does not need to be proxied (working, but not really)
  - [ ] Proxy traffic via ssh
- [ ] Niceties
  - [x] Make a man page
  - [x] Install a dummy config file for the user to use
  - [ ] Detect if a site is blocked (I dont trust the current implimentation)
  - [ ] Account for the ssh server being down (Ducktape and superglue, does not work half the time)