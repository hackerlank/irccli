#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <stdarg.h>

void error(char *msg);
void rl_printf(char *fmt, ...);
void destroy_prompt();

#endif
