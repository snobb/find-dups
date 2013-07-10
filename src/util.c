/*  util.c  */
/*  Copyright (C) 2013 Alex Kozadaev [akozadaev at yahoo com]  */

#include "finddup.h"
#include <stdlib.h>
#include <string.h>

/* simple strdup implementation */
char *util_strdup(const char *s)
{
    char *new;

    new = malloc(strlen(s)+1);
    check_mem(new);
    strcpy(new, s);
    return new;
error:
    return NULL;
}

/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0 list */

