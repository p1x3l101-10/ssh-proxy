# ssh-proxy (WIP)
A refactor of a refactor of a refactor. I never know when to stop...

## What is it?
This current iteration does a couple of things:
- Opens a socks5 proxy on the configured port
- Detects if a site is blocked or not
- If a site is not blocked, it will just relay the traffic as if it was not there
- If a site is blocked, it will proxy the traffic over an ssh server

## Why?
Because `ssh -D` killed my internet. Now I have a program so that it will be smarter about when to proxy the traffic.