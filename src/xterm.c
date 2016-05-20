#include "xterm.h"

static int altb = 0;
static const char
	*smcup   = "\033[?1049h\033[H",          // switch to alternate screen
	*rmcup   = "\033[?1049l",                // switch back to normal screen
	*black   = "\033[0;30m",                 // black color text
	*red     = "\033[0;31m",                 // red color text
	*green   = "\033[0;32m",                 // green color text
	*yellow  = "\033[0;33m",                 // yellow color text
	*blue    = "\033[0;34m",                 // blue color text
	*magenta = "\033[0;35m",                 // magenta color text
	*cyan    = "\033[0;36m",                 // cyan color text
	*white   = "\033[0;37m",                 // white color text
	*reset   = "\033[0m";                    // reset text

int galt() {
	return altb;
}

void alt(int a) {
	altb = a;
}

const char *xget(const char *str) {
	if      (strcmp(str, "smcup"  ) == 0) return smcup;
	else if (strcmp(str, "rmcup"  ) == 0) return rmcup;
	else if (strcmp(str, "black"  ) == 0) return black;
	else if (strcmp(str, "red"    ) == 0) return red;
	else if (strcmp(str, "green"  ) == 0) return green;
	else if (strcmp(str, "yellow" ) == 0) return yellow;
	else if (strcmp(str, "blue"   ) == 0) return blue;
	else if (strcmp(str, "magenta") == 0) return magenta;
	else if (strcmp(str, "cyan"   ) == 0) return cyan;
	else if (strcmp(str, "white"  ) == 0) return white;
	else if (strcmp(str, "reset"  ) == 0) return reset;
	else                                  return "";
}
