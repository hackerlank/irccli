#ifndef SOCK_UTIL_H
#define SOCK_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iconv.h>

#include "util.h"

int getsockfd();

int read_socket(char buffer[512]);
void write_socket(char msg[512]);
void connect_socket(char *server_name, int portno);
void encode_socket(const char *enc);
void close_socket();

#endif
