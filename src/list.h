#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include "common.h"
#include "md5.h"

/* linked list to store the duplicate file names */
struct lnode {
    char *value;
    md5_t *chksum;
    struct lnode *next;
};

/* list.c */
struct lnode *list_add(struct lnode *list, const char *fname);
void list_free(struct lnode *list);

#endif /* __LIST_H__ */
