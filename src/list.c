/*
 *  list.c
 *  author: Aleksei Kozadaev (2015)
 */

#include "list.h"

/* add a filename to the list of duplicates */
struct lnode *list_add(struct lnode *list, const char *fname)
{
    struct lnode *new;

    new = malloc(sizeof(*new));
    CHECK_MEM(new);
    new->value = xstrndup(fname, MAXPATH);
    new->next = list;
    return new;
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

