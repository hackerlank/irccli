#include "xterm.h"

int galt() {
	return altb;
}

void alt(int a) {
	altb = a;
}

const char *xget(const char *str) {
	if      (strcmp(str, "smcup") == 0) return smcup;
	else if (strcmp(str, "rmcup") == 0) return rmcup;
	else                                return "";
}
