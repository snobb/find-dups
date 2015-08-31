/*
 *  hashlist.c
 *  Author: Alex Kozadaev (2015)
 */

#include <string.h>
#include "common.h"
#include "hashlist.h"

#define NHASH           4001
#define MULTIPLIER      31

static struct hashlist **list = NULL;

static size_t hashlist_hash(const char *str);
static struct hashlist *hashlist_bucket_lookup(const md5_t chksum);
static void hashlist_freenode(struct hashlist *list);

/* initialize the hashlist */
void
hashlist_init(void)
{
    list = malloc(sizeof(*list) * NHASH);
    CHECK_MEM(list);
}

/* add a filename to the list of duplicates */
void
hashlist_add(const md5_t chksum, const char *fname)
{
    struct hashlist *head, *new;
    size_t idx = hashlist_hash((const char*)chksum);

    head = list[idx];

    head = hashlist_bucket_lookup(chksum);
    if (!head) {
        new = malloc(sizeof(*new));
        CHECK_MEM(new);

        memcpy((void*)new->chksum, (void*)chksum, MAXCHKSUM);
        head->fnames = array_new();
        new->next = NULL;
        list[idx] = new;
    }

    array_add(head->fnames, fname);
}

void
hashlist_free(void)
{
    for (int i; i < NHASH; i++) {
        hashlist_freenode(list[i]);
    }
    free(list);
}

static size_t
hashlist_hash(const char *str)
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
    size_t idx = hashlist_hash((const char*)chksum);

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
    struct hashlist *p, *next;
    for (p = list; p; p = next) {
        next = p->next;
        array_free(p->fnames);
        free(p);
    }
}

/* vim: set ts=4 sts=8 sw=4 smarttab et si tw=80 cino=t0l1(0k2s fo=crtocl */
