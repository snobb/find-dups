#ifndef __UTIL_H__
#define __UTIL_H__

#include "common.h"
#include <stdlib.h>

void die(const char *errstr, ...);
char *xstrndup(const char *src, size_t max);

#endif /* __UTIL_H__ */
