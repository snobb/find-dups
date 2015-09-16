/*
 *  main.c
 *  Author: Alex Kozadaev (2015)
 */

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include "build_host.h"
#include "common.h"
#include "hashlist.h"

#define check_error(a) if((a) != R_OK) { goto error; }

static void usage(void);
static int print_callback(const char *str);
static int handle_file(char *fname);
static int walk_dir(const char *dir, int (*cb)(char *));

int
main(int argc, char **argv)
{
    if (argc == 1) {
        usage();
    } else {
        hashlist_init();
        while (--argc > 0) {
            if (handle_file(*(++argv)) != R_OK) {
                usage();
                die("error: invalid agruments\n");
            }
        }
        hashlist_finddups(print_callback);
        hashlist_free();
    }
    return EXIT_SUCCESS;
}

static void
usage(void)
{
    puts("finddup v" BUILD_VERSION "\n"
         "Usage: finddup [directory/files to search]\n");
}

/* file name printing callback */
static int
print_callback(const char *str)
{
    return printf("\t%s\n", str);
}

/* handle the files in the given directory
 * calls walk_dir in case a directory is found and recursively
 * looking through in the subdirectories */
static int
handle_file(char *fname)
{
    struct stat stbuf;
    md5_t chksum;

    if (stat(fname, &stbuf) == -1) {
        perror(strerror(errno));
        goto error;
    }

    if (S_ISDIR(stbuf.st_mode)) {
        check_error(walk_dir(fname, handle_file));
    } else {
        check_error(md5_get(fname, chksum));
        hashlist_add(chksum, fname);
    }

    return R_OK;

error:
    return R_ERR;
}

/* got to the directory, make necessary checks and call
 * (handle_files) to handle the contents */
static int
walk_dir(const char *dir, int (*cb)(char *))
{
    char name[MAXPATH];
    struct dirent *dp;
    DIR *dfd;
    size_t dirlen;

    if ((dfd = opendir(dir)) == NULL) {
        goto error;
    }

    dirlen = strlen(dir);
    while ((dp = readdir(dfd)) != NULL) {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
            continue;
        }
        if ((dirlen + strlen(dp->d_name)+2) > MAXPATH) {
            goto error;
        } else {
            if (dp->d_type == DT_REG || dp->d_type == DT_DIR) {
                sprintf(name, "%s/%s", dir, dp->d_name);
                cb(name);
            }
        }
    }
    closedir(dfd);
    return R_OK;

error:
    if (dfd) { closedir(dfd); }
    perror(strerror(errno));
    return R_ERRDIR;
}

/* vim: set ts=4 sts=8 sw=4 smarttab et si tw=80 cino=t0l1(0k2s fo=crtocl */
