/*
 *  hashlist.c
 *  Author: Alex Kozadaev (2015)
 */

#include <string.h>
#include "common.h"
#include "md5.h"
#include "hashlist.h"

#define NHASH           4001
#define MULTIPLIER      31

static struct hashlist **list = NULL;

static size_t hashlist_hash(const unsigned char *str);
static struct hashlist *hashlist_bucket_lookup(const md5_t chksum);
static void hashlist_freenode(struct hashlist *list);

/* initialize the hashlist */
void
hashlist_init(void)
{
    list = calloc(sizeof(*list), NHASH);
    CHECK_MEM(list);
}

/* add a filename to the list of duplicates */
void
hashlist_add(const md5_t chksum, const char *fname)
{
    struct hashlist *head, *new;
    size_t idx = hashlist_hash(chksum);

    head = hashlist_bucket_lookup(chksum);
    if (head) {
        head->fnames = list_add(head->fnames, fname);
    } else {
        new = malloc(sizeof(*new));
        CHECK_MEM(new);

        md5_copy(new->chksum, chksum);
        new->fnames = list_add(NULL, fname);
        new->next = list[idx];
        list[idx] = new;
    }
}

void
hashlist_finddups(int (*cb)(const char*))
{
    struct hashlist *ptr;
    struct lnode *fnames;
    for (int i = 0; i < NHASH; ++i) {
        for (ptr = list[i]; ptr != NULL; ptr = ptr->next) {
            fnames = ptr->fnames;
            /* display only if there are at least 2 records */
            if (fnames && fnames->next) {
                md5_print(ptr->chksum);
                for (; fnames != NULL; fnames = fnames->next) {
                    cb(fnames->value);
                }
            }
        }
    }
}

void
hashlist_free(void)
{
    for (int i = 0; i < NHASH; i++) {
        hashlist_freenode(list[i]);
    }
    free(list);
}

static size_t
hashlist_hash(const unsigned char *str)
{
    size_t p = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        p += MULTIPLIER * p + str[i];
    }
    return p % NHASH;
}

static struct hashlist *
hashlist_bucket_lookup(const md5_t chksum)
{
    struct hashlist *head, *ptr;
    size_t idx = hashlist_hash(chksum);

    if (!list[idx]) {
        return NULL;    /* bucket does not exist */
    }

    head = list[idx];
    for (ptr = head; ptr != NULL; ptr = ptr->next) {
        if (md5_compare(ptr->chksum, chksum) == 0) {
            return ptr;
        }
    }

    return NULL;
}

/* free list structure */
static void
hashlist_freenode(struct hashlist *list)
{
    struct hashlist *ptr, *next = list;
    for (ptr = list; ptr != NULL; ptr = next) {
        next = ptr->next;
        list_free(ptr->fnames);
        free(ptr);
    }
}

/* vim: set ts=4 sts=8 sw=4 smarttab et si tw=80 cino=t0l1(0k2s fo=crtocl */
