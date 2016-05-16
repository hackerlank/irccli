# irccli
A minimal command line IRC client

## About

Irccli is a simple command line irc client written from scratch in C. It is currently still in development.

## Compiling
> _Note: Must have [pcre](http://www.pcre.org/) and [readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html) installed to compile correctly_

Simply run `./configure` and then `make`.

## Todo
- `/list` command no longer printing
- Log sent messages (messages sent from the user are not printed or logged)
- Add support for printing all UTF-8 characters (e.g., Chinese characters)
- Add `/channel` functionality for private messages from different users
- Add color support
- Add support for user actions (`user: ACTION lols -> * user lols`)
- Only print `QUIT` messages for the channel the user quit from (multiple if necessary)
 - Maybe use structs for each channel and keep an array of users
