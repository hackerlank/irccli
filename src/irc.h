#ifndef IRC_H
#define IRC_H

#include <pcre.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util.h"
#include "sock_util.h"

// Channels names are strings (beginning with a '&' or '#' character)
// of length up to 200 characters.
// (https://tools.ietf.org/html/rfc1459#section-1.3)
static char current_channel[256];
static char nick[512];
static char user[512];
static char real[512];

void irc_nick();
void irc_user();

int irc_parse(char src[512], char ***output);
int irc_receive(char *buffer);
int irc_send(char *buffer);

#endif
