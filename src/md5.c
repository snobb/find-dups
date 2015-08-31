/*  md5.c  */
/*  Copyright (C) 2013 Alex Kozadaev [akozadaev at yahoo com]  */

#include <stdlib.h>
#include "md5.h"

#define BUFSZ 8192 /* MD5_CBLOCK * 128 */

/* generate an md5 checksum from the file contents */
int md5_get(const char *fname, md5_t retsum)
{
    MD5_CTX c;
    char buf[BUFSZ];
    size_t bytes;
    FILE *fd;

    if ((fd = fopen(fname, "r")) == NULL) {
        return R_ERR;
    }

    MD5_Init(&c);
    while((bytes=fread(buf, 1,  BUFSZ, fd)) > 0)
    {
        MD5_Update(&c, buf, bytes);
    }
    fclose(fd);

    MD5_Final(retsum, &c);
    return R_OK;
}

/* print the hex checksum to the stdout */
void md5_print(const md5_t chksum)
{
    int i;
    for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
        printf("%02x", chksum[i]);
    }
}

/* compare two checksums */
int md5_compare(const md5_t cs1, const md5_t cs2)
{
    int n = 0;
    const unsigned char *p1 = cs1, *p2 = cs2;
    while (*p1 == *p2 && ++n < MD5_DIGEST_LENGTH) {
        p1++;
        p2++;
    }
    return *p1 - *p2;
}

/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0 list */

