#ifndef XTERM_H
#define XTERM_H

#include <string.h>

static int altb = 0;
int galt();
void alt(int a);

static const char
	*smcup   = "\033[?1049h\033[H",          // switch to alternate screen
	*rmcup   = "\033[?1049l",                // switch back to normal screen
	*red     = "\033[0;31m",                 // red color text
	*green   = "\033[0;32m",                 // green color text
	*yellow  = "\033[0;33m",                 // yellow color text
	*blue    = "\033[0;34m",                 // blue color text
	*magenta = "\033[0;35m",                 // magenta color text
	*cyan    = "\033[0;36m",                 // cyan color text
	*reset   = "\033[0m";                    // reset text

const char *xget(const char *str);

#endif
