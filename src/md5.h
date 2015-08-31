#ifndef MD5_H
#define MD5_H

/*    Desclarations    */
#include <openssl/md5.h>
#include "common.h"

/* hex digest (1 byte == 2 chars) + end of line */
#define MAXCHKSUM       (MD5_DIGEST_LENGTH * 2 + 1)

/* storage for the md5 checksum */
typedef unsigned char md5_t[MD5_DIGEST_LENGTH];

int md5_get(const char *fname, md5_t retsum);
void md5_print(const md5_t chksum);
int md5_compare(const md5_t cs1, const md5_t cs_2);

#endif /* MD5_H */
