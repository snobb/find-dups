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

static LIST_HEAD(registry, hashnode) *list = NULL;

static size_t hasharray_hash(const unsigned char *str);
static struct hashnode *hasharray_lookup(const md5_t chksum);
static void hasharray_freenode(struct registry *node);

/* initialize the hasharray */
void
hasharray_init(void)
{
    /* no need to HEAD_INIT since we nullify the storage */
    list = calloc(sizeof(*list), NHASH);
    CHECK_MEM(list);
}

/* add a filename to the list of duplicates */
void
hasharray_add(const md5_t chksum, const char *fname)
{
    struct hashnode *head;

    head = hasharray_lookup(chksum);
    if (!head) {
        head = calloc(sizeof(*head), 1);
        CHECK_MEM(head);

        size_t idx = hasharray_hash(chksum);
        md5_copy(head->chksum, chksum);
        head->fnames = array_new();

        LIST_INSERT_HEAD(&list[idx], head, entries);
    }

    array_add(head->fnames, fname);
}

void
hasharray_finddups(int (*cb)(const char*))
{
    struct hashnode *ptr;
    struct array *array;
    for (int i = 0; i < NHASH; ++i) {
        LIST_FOREACH(ptr, &list[i], entries) {
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
    struct registry *lst = list;
    for (int i = 0; i < NHASH; ++i) {
        hasharray_freenode(lst++);
    }
    free(list);
}

static size_t
hasharray_hash(const unsigned char *str)
{
    size_t p = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        p += MULTIPLIER * p + str[i];
    }
    return p % NHASH;
}

static struct hashnode *
hasharray_lookup(const md5_t chksum)
{
    struct hashnode *ptr;

    size_t idx = hasharray_hash(chksum);

    if (!list[idx].lh_first) {
        return NULL;    /* bucket does not exist */
    }

    LIST_FOREACH(ptr, &list[idx], entries) {
        if (md5_compare(ptr->chksum, chksum) == 0) {
            return ptr;
        }
    }

    return NULL;
}

/* free node structure (linked list) */
static void
hasharray_freenode(struct registry *node)
{
    struct hashnode *ptr = node->lh_first,
                    *ptr_next = NULL;

    for (ptr = node->lh_first; ptr; ptr = ptr_next) {
        ptr_next = ptr->entries.le_next;
        LIST_REMOVE(ptr, entries);
        array_free(ptr->fnames);
        free(ptr);
    }
}

