# irccli
A minimal command line IRC client

## About

Irccli is a simple command line irc client written from scratch in C. It is currently still in development.

## Compiling
> _Note: Must have [pcre](http://www.pcre.org/) and [readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html) installed to compile correctly_

Simply run `./configure` and then `make`.

## Todo
- Finish implementing user commands
- Better parse messages received from irc server (i.e., output from commands like PART)
 - Better parse messages from other users (e.g., "h4x0r: Message")
- Log other channels' output which the user is connected to, but only print when the user switches to that channel
