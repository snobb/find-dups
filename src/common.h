#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define _XOPEN_SOURCE 700

#define MAXPATH 1024

#define CHECK_MEM(a)   if (!a) { die("error: out of memory\n"); }

enum { R_OK, R_ERR, R_ERRDIR };

#endif /* __COMMON_H__ */
