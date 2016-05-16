#ifndef XTERM_H
#define XTERM_H

#include <string.h>

static int altb = 0;
int galt();
void alt(int a);

static const char
	*smcup = "\033[?1049h\033[H",          // switch to alternate screen
	*rmcup = "\033[?1049l";                // switch back to normal screen

const char *xget(const char *str);

#endif
