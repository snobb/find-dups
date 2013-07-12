/*  list.c  */
/*  Copyright (C) 2013 Alex Kozadaev [akozadaev at yahoo com]  */

#include "finddup.h"

/* add a filename to the list of duplicates */
struct lnode *list_add(struct lnode *list, const char *fname)
{
    struct lnode *new;

    new = malloc(sizeof(*new));
    check_mem(new);
    new->value = util_strdup(fname);
    new->next = list;
    return new;
error:
    return NULL;
}

/* free list structure */
void list_free(struct lnode *list)
{
    struct lnode *p, *next;
    for (p = list; p; p = next) {
        next = p->next;
        free(p->value);
        free(p);
    }
}



/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0 list */

