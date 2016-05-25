# irccli
A minimal command line IRC client

## About

Irccli is a simple command line irc client written from scratch in C. It is currently still in development.

## Screenshot
![image](http://i.imgur.com/yKEYP6R.gif)

## Compile
> _Note: Must have [pcre](http://www.pcre.org/) and [readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html) installed to compile correctly_

`./configure && make`

## Todo
- [ ] Add `/nick` command to change nickname
- [ ] Add support for bold, underline, (colors?)
- [ ] Add automatic Nickserv with specified password
- [ ] Add `/channel` functionality that if no channel is specified, go to most recent channel
- [ ] Remove the prompt input from being printed after hit enter
- [ ] Add `/channel` functionality for private messages from different users (and fix `/msg` functionality)
- [ ] Print (and log) `QUIT` messages to the channel(s) the user was on (use structs?)
- [ ] Add (readline) autocomplete for usernames on the channel

## License
GPLv3
