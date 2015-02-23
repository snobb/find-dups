/*  test_md5.c  */
/*  Copyright (C) 2013 Alex Kozadaev [akozadaev at yahoo com]  */

#include "minunit.h"
#include "../src/finddup.h"
#include <stdio.h>
#include <string.h>

#define DEBUG 1

static char *test_md5_compare()
{
    int res = 0;
    unsigned char m1[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    unsigned char m2[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    };
    unsigned char m3[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    unsigned char m4[] = {
        0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    res = md5_compare(m1, m2);
    mu_assert_msg(res < 0, "Expected m1 is less the m2")
    res = md5_compare(m3, m4);
    mu_assert_msg(res < 0, "Expected m3 is less the m4")

    return NULL;
}

static char *test_md5_get()
{
    unsigned char got[16], expected[16] = {
        0x46, 0x3b, 0x70, 0x08, 0xd0, 0xbd, 0xe3, 0xa7,
        0xc7, 0x11, 0x99, 0x89, 0xc4, 0xa6, 0x02, 0x8c,
    };
    char md5exp[] = "463b7008d0bde3a7c7119989c4a6028c";
    char md5got[33];

    md5_get("test/testfile", got);
    mu_assert_msg(md5_compare(got, expected) == 0, "md5sum mismatch")

    for (int i = 0; i < 16; i++) {
        sprintf(md5got+(i*2), "%02x", got[i]);
    }
    mu_assert_msg(strncmp(md5exp, md5got, 32) == 0, "md5sum mismatch")

    return NULL;
}

static char *all_tests() {
    /* test_md5.c */
    mu_run_test(test_md5_compare);
    mu_run_test(test_md5_get);

    return NULL;
}

RUN_TESTS(all_tests)


/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0 list */

