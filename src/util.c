#include "util.h"

void error(char *msg) {
	perror(msg);
	exit(1);
}
