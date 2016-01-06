#ifndef __HASHLIST_H__
#define __HASHLIST_H__

#include <stdlib.h>
#include <sys/queue.h>
#include "common.h"
#include "md5.h"
#include "array.h"

/* hashmap node */
struct hashnode {
    md5_t chksum;
    struct array *fnames;
    LIST_ENTRY(hashnode) entries;
};

/* hasharray.c */
void hasharray_init(void);
void hasharray_add(const md5_t chksum, const char *fname);
void hasharray_finddups(int (*cb)(const char*));
void hasharray_free(void);

#endif /* __HASHLIST_H__ */
