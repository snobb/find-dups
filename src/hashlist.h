#ifndef __HASHLIST_H__
#define __HASHLIST_H__

#include <stdlib.h>
#include "common.h"
#include "md5.h"
#include "array.h"

/* hashmap node */
struct hashlist {
    const md5_t chksum;
    struct array *fnames;
    struct hashlist *next;
};

/* hashlist.c */
void hashlist_init(void);
void hashlist_add(const md5_t chksum, const char *fname);
void hashlist_free(void);


#endif /* __HASHLIST_H__ */
