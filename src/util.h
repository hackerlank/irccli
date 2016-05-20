#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <stdarg.h>

#include "xterm.h"

void error(char *msg);
void rl_printf(char *fmt, ...);
void destroy_prompt();

char *scolor(char *str, const char *color);

uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed);

#endif
