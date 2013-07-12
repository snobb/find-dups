#ifndef FINDDUP_H
#define FINDDUP_H

/*    Desclarations    */
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>

#define VERSION "0.02"

#define check_mem(a)   if (!a) { fputs("error: out of memory", stderr);\
                                    goto error; }

enum { R_OK, R_ERR, R_ERRDIR };

/* storage for the md5 checksum */
typedef unsigned char md5_t[MD5_DIGEST_LENGTH];

/* linked list to store the duplicate file names */
struct lnode {
    char *value;
    struct lnode *next;
};

/* tree data structure */
struct tnode {
    md5_t chksum;
    unsigned count;
    struct lnode *names;
    struct tnode *left;
    struct tnode *right;
};

/* tree.c */
void tree_add(const char *fname, const md5_t chksum);
void tree_finddups(int morethen, int (*cb)(const char *));
void tree_free(void);

/* list.c */
struct lnode *list_add(struct lnode *list, const char *fname);
void list_free(struct lnode *list);

/* md5.c */
int md5_get(const char *fname, md5_t retsum);
void md5_print(const md5_t chksum);
int md5_compare(const md5_t cs1, const md5_t cs_2);

/* util.c */
char *util_strdup(const char *s);

#endif /* FINDDUP_H */
