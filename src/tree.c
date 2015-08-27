/*  tree.c  */
/*  Copyright (C) 2013 Alex Kozadaev [akozadaev at yahoo com]  */

#include "finddup.h"
#include <stdio.h>
#include <string.h>

static struct tnode *root = NULL;

static void tree_add_rec(struct tnode **root, const char *fname,
                         const md5_t chksum);
static void tree_finddups_rec(struct tnode *root, int morethen,
                              int (*cb)(const char *));
static void tree_free_rec(struct tnode *root);

size_t max_bucket_depth = 0;

void tree_add(const char *fname, const md5_t chksum)
{
    tree_add_rec(&root, fname, chksum);
}

void tree_finddups(int morethen, int (*cb)(const char *))
{
    tree_finddups_rec(root, morethen, cb);
}

void tree_free()
{
    tree_free_rec(root);
}

/* ==[ static functions ]=================================================== */

/* recursive adding to the tree */
static void tree_add_rec(struct tnode **root_branch, const char *fname,
                         const md5_t chksum)
{
    int cmpres = 0;
    struct tnode *root, *node;

    root = *root_branch;
    if (!root) {
        node = malloc(sizeof(*node));
        node->names = list_add(NULL, fname);
        node->left = node->right = NULL;
        node->count = 1;
        memcpy(node->chksum, chksum, MD5_DIGEST_LENGTH);
        *root_branch = node;
    } else {
        cmpres = md5_compare(root->chksum, chksum);
        if (cmpres == 0) {
            root->names = list_add(root->names, fname);
            root->count++;
        } else if (cmpres < 0) {
            tree_add_rec(&root->left, fname, chksum);
        } else {
            tree_add_rec(&root->right, fname, chksum);
        }
    }
}

/* find duplicates in the tree that is higher then (morethen)
 * If duplicates found - print md5sum of the files and an indented list of
 * files below
 */
static void tree_finddups_rec(struct tnode *root, int morethen,
                              int (*cb)(const char *))
{
    size_t depth = 0;
    struct lnode *p;

    if (!root) {
        return;
    }

    tree_finddups_rec(root->left, morethen, cb);
    if (root->count > morethen) {
        md5_print(root->chksum);
        printf("\n");
        for (p = root->names; p; p=p->next) {
            cb(p->value);
            ++depth;
        }
    }
    tree_finddups_rec(root->right, morethen, cb);
}

/* free the tree structure */
static void tree_free_rec(struct tnode *root)
{
    if (!root) {
        return;
    }
    tree_free_rec(root->left);
    tree_free_rec(root->right);
    list_free(root->names);
    free(root);
}

/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0 list */

