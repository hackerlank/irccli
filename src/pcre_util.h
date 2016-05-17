#ifndef PCRE_UTIL_H
#define PCRE_UTIL_H

#include <pcre.h>
#include <string.h>

#include "util.h"

int re_match(char *src, const char *regex, char ***output, int caseless);

#endif
