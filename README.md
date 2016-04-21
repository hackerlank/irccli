# irccli
A minimal command line IRC client

## About

Irccli is a simple command line irc client written from scratch in C. It is currently still in development.

## Compiling
> _Note: Must have [pcre](http://www.pcre.org/) and [readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html) installed to compile correctly_

Simply run `./configure` and then `make`.

## Todo
- Parse and handle user sent messages
 - Add alias functions the user can type (e.g., /join instead of JOIN)
- Better parse messages received from irc server (i.e., output from commands like PART)
