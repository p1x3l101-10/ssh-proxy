# What needs to happen before release

- [x] Get a working socks server
  - [x] Relay traffic
  - [x] Detect if a site is blocked (I dont trust the current implimentation)
  - [x] Proxy traffic via ssh
- [ ] Niceties
  - [x] Make a man page
  - [x] Install a dummy config file for the user to use
  - [x] Show build settings
  - [ ] Ssh compression
  - [ ] Account for the ssh server being down (Ducktape and superglue, does not work half the time)
- [x] Config file
  - [x] Use the new standard of having a defaults file, and a user file
  - [x] Supply a template with all the options