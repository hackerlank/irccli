#include "xterm.h"

int galt() {
	return altb;
}

void alt(int a) {
	altb = a;
}

const char *xget(const char *str) {
	if      (strcmp(str, "smcup"  ) == 0) return smcup;
	else if (strcmp(str, "rmcup"  ) == 0) return rmcup;
	else if (strcmp(str, "red"    ) == 0) return red;
	else if (strcmp(str, "green"  ) == 0) return green;
	else if (strcmp(str, "yellow" ) == 0) return yellow;
	else if (strcmp(str, "blue"   ) == 0) return blue;
	else if (strcmp(str, "magenta") == 0) return magenta;
	else if (strcmp(str, "cyan"   ) == 0) return cyan;
	else if (strcmp(str, "reset"  ) == 0) return reset;
	else                                  return "";
}
