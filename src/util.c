/*  util.c  */
/*  Copyright (C) 2013 Alex Kozadaev [akozadaev at yahoo com]  */

#include "finddup.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void die(const char *errstr, ...)
{
    va_list ap;

    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(1);
}

inline char *xstrndup(const char *src, size_t max)
{
    char *dst;
    int n = strlen(src + 1);

    if (n > max) {
        n = max;
    }

    dst = malloc(n);
    if (!dst) {
        die("xstrndup: out of memory\n");
    }
    memcpy(dst, src, n - 1);
    dst[n - 1] = '\0';

    return dst;
}

/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0 list */

