#ifndef ENCODE_H
#define ENCODE_H

#include <string.h>
#include <iconv.h>

#include "util.h"

char *genc();
void enc(char *encoding);

void   encode(char **str, int size);
void unencode(char **str, int size);

#endif
