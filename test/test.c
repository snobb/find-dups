/*  test.c  */
/*  Copyright (C) 2013 Alex Kozadaev [akozadaev at yahoo com]  */

#include "minunit.h"

char *test_md5_compare();
char *test_md5_get();

char *all_tests() {
    mu_suite_start();

    /* test_md5.c */
    mu_run_test(test_md5_compare);
    mu_run_test(test_md5_get);

    return NULL;
}

RUN_TESTS(all_tests)

/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0 list */

