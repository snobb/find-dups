#ifndef MINUNIT_H
#define MINUNIT_H


/* ==[ example use ]========================================================

#include "minunit.h"

char *test_dlopen()
{
    return NULL;
}

char *test_functions()
{
    return NULL;
}

char *test_failures()
{
    return NULL;
}

char *test_dlclose()
{
    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_dlopen);
    mu_run_test(test_functions);
    mu_run_test(test_failures);
    mu_run_test(test_dlclose);

    return NULL;
}

RUN_TESTS(all_tests);

========================================================================= */

/*    Desclarations    */
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#       define debug(fmt, ...) fprintf(stderr,"[DEBUG] %s:%d " fmt,\
                                       __FILE__, __LINE__, __VA_ARGS__)
#else
#       define debug(fmt, ...)
#endif

#define mu_suite_start() char *message __attribute__ ((unused)) = NULL;
#define mu_assert(test, message) if (!(test)) {\
                                    if (message)\
                                       return message " (" #test ")";\
                                    else return message;\
                                 }
#define mu_run_test(test) { char *message = test(); tests_run++; \
                                     if (message) return message; }

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
    fprintf(stderr, "----\nRUNNING: %s\n", argv[0]);\
    char *result = name();\
    if (result != 0) {\
        fprintf(stderr, "FAILED: %s\n", result);\
    } else {\
        fprintf(stderr, "ALL TESTS PASSED\n");\
    }\
    fprintf(stderr, "Tests run: %d\n", tests_run);\
    exit(result != 0);\
}

int tests_run;

#endif /* MINUNIT_H */
