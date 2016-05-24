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
#include "encode.h"

int  irc_init(char *serv, int log, char *nck, char *usr, char *rl);
int  irc_receive(char *buffer, int R);
int  irc_send(char *buffer);
void irc_clean();

void setup_log(char **lname, int lsize, char *dest);

#endif
