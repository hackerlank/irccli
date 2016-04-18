#ifndef SOCK_UTIL_H
#define SOCK_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netdb.h>

#include "util.h"

void sockwrite(int sockfd, char msg[512]);
void connect_socket(int *sockfd, char *server_name, int portno);

#endif
