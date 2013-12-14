/*  main.c  */
/*  Copyright (C) 2013 Alex Kozadaev [akozadaev at yahoo com]  */

#include "finddup.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#define CHECK_ERROR(A) if((A) != R_OK) { goto error; }

#define NTHREAD 10

static pthread_mutex_t g_mutex;
static pthread_cond_t g_condv;

static void usage(void);
static int print_callback(const char *str);
static int handle_file(char *fname);
static int walk_dir(const char *dir, int (*cb)(char *));

/* create/destroy the workqueue item */
void wqueue_item_destroy(struct wqueue_item *item);
struct wqueue_item *wqueue_new_item(char *data,
                                    void (*destroy)(struct wqueue_item*));

void *file_handler(void *arg);  /* file handler thread */

int main(int argc, char **argv)
{
    struct thread_ctx thread[NTHREAD];

    /* initializing the work queue */
    wqueue_init(&g_mutex, &g_condv);

    /* create consumer threads that will process data in the queue */
    for (int i = 0; i < NTHREAD; i++) {
        thread_init(&thread[i]);
        if (thread_start(&thread[i], file_handler, &thread[i]) != 0) {
            fputs("error: cannot create threads", stderr);
            exit(1);
        }
    }

    if (argc == 1) {
        usage();
    } else {
        while (--argc > 0) {
            CHECK_ERROR(handle_file(*++argv));
        }
    }

    /* sending NULL to terminte the threads we all done */
    for (int i = 0; i < NTHREAD; i++) {
        wqueue_enqueue(NULL);
    }

    /* joining the theads and waiting they are all gone */
    for (int i = 0; i < NTHREAD; i++) {
        thread_join(&thread[i]);
    }

    /* printing the results */
    tree_finddups(1, print_callback);

error:
    tree_free();
    wqueue_destroy();
    return 0;
}

static void usage(void)
{
    puts("finddup v" VERSION "\n"
         "Usage: finddup [directory/files to search]\n");
}

/* file name printing callback */
static int print_callback(const char *str)
{
    return printf("\t%s\n", str);
}

/* handle the files in the given directory
 * calls walk_dir in case a directory is found and recursively
 * looking through in the subdirectories */
static int handle_file(char *fname)
{
    struct stat stbuf;

    if (stat(fname, &stbuf) == -1) {
        perror(strerror(errno));
        return R_ERR;
    }

    if (S_ISDIR(stbuf.st_mode)) {
        CHECK_ERROR(walk_dir(fname, handle_file));
    } else {
        wqueue_enqueue(wqueue_new_item(util_strdup(fname),
                                        wqueue_item_destroy));
    }
    return R_OK;
error:
    return R_ERR;
}

/* got to the directory, make necessary checks and call
 * (handle_files) to handle the contents */
static int walk_dir(const char *dir, int (*cb)(char *))
{
    char name[MAXPATH];
    struct dirent *dp;
    DIR *dfd;
    size_t dirlen;

    if ((dfd = opendir(dir)) == NULL) {
        perror(strerror(errno));
        return R_ERRDIR;
    }

    dirlen = strlen(dir);
    while ((dp = readdir(dfd)) != NULL) {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
            continue;
        }
        if ((dirlen + strlen(dp->d_name)+2) > MAXPATH) {
            perror("error: filename is too long");
            return R_ERRDIR;
        } else {
            if (dp->d_type == DT_REG || dp->d_type == DT_DIR) {
                sprintf(name, "%s/%s", dir, dp->d_name);
                cb(name);
            }
        }
    }
    closedir(dfd);
    return R_OK;
}

void *file_handler(void *arg)
{
    struct thread_ctx *thread;
    struct wqueue_item *item;
    md5_t chksum;

    thread = (struct thread_ctx *)arg;

    while(true) {
        item = wqueue_dequeue();

        if (item == NULL) {
            return NULL;     /* the thread is done */
        }

        CHECK_ERROR(md5_get(item->data, chksum));
        pthread_mutex_lock(&g_mutex);
        tree_add(item->data, chksum);
        pthread_mutex_unlock(&g_mutex);
        item->destroy(item);
error:
        continue;
    }
    return NULL;
}

/* make a new data container */
struct wqueue_item *wqueue_new_item(char *data,
                                    void (*destroy)(struct wqueue_item *))
{
    struct wqueue_item *new;
    new = malloc(sizeof(*new));
    CHECK_MEM(new);
    new->data = data;
    new->destroy = destroy;
    return new;

error:
    exit(1);
}

/* destroy the workqueue item container */
void wqueue_item_destroy(struct wqueue_item *item)
{
    free(item->data);
    free(item);
}

/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0 list */

