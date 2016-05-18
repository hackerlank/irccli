# irccli
A minimal command line IRC client

## About

Irccli is a simple command line irc client written from scratch in C. It is currently still in development.

## Compiling
> _Note: Must have [pcre](http://www.pcre.org/) and [readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html) installed to compile correctly_

Simply run `./configure` and then `make`.

## Todo
- Add automatic Nickserv with specified password
- Add `/channel` functionality that if no channel is specified, go to most recent channel
- Remove the prompt input from being printed after hit enter
- Add `/channel` functionality for private messages from different users
 - Fix `/msg` functionality (does not print or switch to different "channel" mode)
- Only print `QUIT` messages for the channel the user quit from (multiple if necessary)
 - Maybe use structs for each channel and keep an array of users
