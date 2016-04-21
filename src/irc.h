#ifndef IRC_H
#define IRC_H

#include <pcre.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util.h"
#include "sock_util.h"

int irc_parse(char src[512], char ***output);
void irc_receive(char *buffer);
void irc_send(char *buffer);

#endif
