/*
 *  hasharray.c
 *  Author: Alex Kozadaev (2015)
 */

#include <string.h>
#include "common.h"
#include "md5.h"
#include "hasharray.h"

#define NHASH           4001
#define MULTIPLIER      31

static struct hasharray **list = NULL;

static size_t hasharray_hash(const unsigned char *str);
static struct hasharray *hasharray_bucket_lookup(const md5_t chksum);
static void hasharray_freenode(struct hasharray *list);

/* initialize the hasharray */
void
hasharray_init(void)
{
    list = calloc(sizeof(*list), NHASH);
    CHECK_MEM(list);
}

/* add a filename to the list of duplicates */
void
hasharray_add(const md5_t chksum, const char *fname)
{
    struct hasharray *head, *new;
    size_t idx = hasharray_hash(chksum);

    head = hasharray_bucket_lookup(chksum);
    if (!head) {
        new = malloc(sizeof(*new));
        CHECK_MEM(new);

        md5_copy(new->chksum, chksum);
        new->fnames = array_new();
        new->next = list[idx];
        head = list[idx] = new;
    }

    array_add(head->fnames, fname);
}

void
hasharray_finddups(int (*cb)(const char*))
{
    struct hasharray *ptr;
    struct array *array;
    for (int i = 0; i < NHASH; ++i) {
        for (ptr = list[i]; ptr != NULL; ptr = ptr->next) {
            array = ptr->fnames;
            if (array->size > 1) {
                md5_print(ptr->chksum);
                for (int j = 0; j < array->size; ++j) {
                    cb(array->values[j]);
                }
            }
        }
    }
}

void
hasharray_free(void)
{
    for (int i = 0; i < NHASH; i++) {
        hasharray_freenode(list[i]);
    }
    free(list);
}

static size_t
hasharray_hash(const unsigned char *str)
{
    size_t p = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        p += MULTIPLIER * p + str[i];
    }
    return p % NHASH;
}

static struct hasharray *
hasharray_bucket_lookup(const md5_t chksum)
{
    struct hasharray *head, *ptr;
    size_t idx = hasharray_hash(chksum);

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
hasharray_freenode(struct hasharray *list)
{
    struct hasharray *ptr, *next = list;
    for (ptr = list; ptr != NULL; ptr = next) {
        next = ptr->next;
        array_free(ptr->fnames);
        free(ptr);
    }
}

/* vim: set ts=4 sts=8 sw=4 smarttab et si tw=80 cino=t0l1(0k2s fo=crtocl */
