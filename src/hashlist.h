#ifndef __HASHLIST_H__
#define __HASHLIST_H__

#include <stdlib.h>
#include "common.h"
#include "md5.h"
#include "list.h"

/* hashmap node */
struct hashlist {
    md5_t chksum;
    struct lnode *fnames;
    struct hashlist *next;
};

/* hashlist.c */
void hashlist_init(void);
void hashlist_add(const md5_t chksum, const char *fname);
void hashlist_finddups(int (*cb)(const char*));
void hashlist_free(void);


#endif /* __HASHLIST_H__ */
