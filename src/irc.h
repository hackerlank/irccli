#ifndef IRC_H
#define IRC_H

#include <pcre.h>
#include <stdlib.h>
#include <string.h>

int irc_parse(char src[512], char ***output);

#endif
