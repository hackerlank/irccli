#ifndef IRC_H
#define IRC_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <glob.h>

#include "util.h"
#include "xterm.h"
#include "pcre_util.h"
#include "sock_util.h"

void irc_nick();
void irc_user();

void irc_init();
int irc_parse(char src[512], char ***output);
int irc_receive(char *buffer, int R);
int irc_send(char *buffer);

void irc_clean();

#endif
