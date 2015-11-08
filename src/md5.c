/*
 *  md5.c
 *  author: Aleksei Kozadaev (2013)
 */

#include <stdlib.h>
#include <string.h>
#include "md5.h"

#define BUFSZ 8192 /* MD5_CBLOCK * 128 */

/* generate an md5 checksum from the file contents */
int
md5_get(const char *fname, md5_t retsum)
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
void
md5_print(const md5_t chksum)
{
    int i;
    for (i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        printf("%02x", chksum[i]);
    }
    putc('\n', stdout);
}

void
md5_copy(md5_t dst, const md5_t src)
{
    memcpy(dst, src, MD5_DIGEST_LENGTH);
}

/* compare two checksums */
int
md5_compare(const md5_t cs1, const md5_t cs2)
{
    int n = 1;
    const unsigned char *p1 = cs1, *p2 = cs2;
    while (*p1 == *p2 && ++n < MD5_DIGEST_LENGTH) {
        ++p1;
        ++p2;
    }
    return *p1 - *p2;
}

