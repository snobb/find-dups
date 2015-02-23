#ifndef MINUNIT_H
#define MINUNIT_H


/* ==[ example use ]========================================================

#include "minunit.h"

static void setup(void)
{
    // setup code here
}

static void teardown(void)
{
    // teardown code here
}

static char *test_dlopen()
{
    return NULL;
}

static char *test_functions(void)
{
    return NULL;
}

static char *test_failures(void)
{
    return NULL;
}

static char *test_dlclose(void)
{
    return NULL;
}

static char *all_tests(void)
{
    // registing setup and teardown functions
    mu_register_setup(setup)
    mu_register_teardown(teardown);

    // running tests
    mu_run_test(test_dlopen);
    mu_run_test(test_functions);
    mu_run_test(test_failures);
    mu_run_test(test_dlclose);

    return NULL;
}

RUN_TESTS(all_tests)

========================================================================= */

/*    Desclarations    */
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#       define debug(fmt, ...) fprintf(stderr,"[DEBUG] %s:%d " fmt,     \
                                       __FILE__, __LINE__, __VA_ARGS__)
#else
#       define debug(fmt, ...)
#endif

#define mu_assert(test)                                                 \
    if (!(test)) { return #test; }

#define mu_assert_msg(test, msg)                                        \
    if (!(test)) {                                                      \
        if (msg) { return msg " (" #test ")"; }                         \
        else { return msg; }                                            \
    }

#define mu_run_test(test)                                               \
    {                                                                   \
        fprintf(stderr, "-- running: %s\n", #test);                     \
        if (setup_func) { setup_func(); }                               \
        char *msg = test();                                             \
        tests_run++;                                                    \
        if (teardown_func) { teardown_func(); }                         \
        if (msg) return msg;                                            \
    }

#define mu_register_setup(setup)           { setup_func = setup; }
#define mu_register_teardown(teardown)     { teardown_func = teardown; }

#define RUN_TESTS(name)                                                 \
    int main(int argc, char *argv[]) {                                  \
        fprintf(stderr, "----\nRUNNING: %s\n", argv[0]);                \
        char *result = name();                                          \
        if (result != 0) {                                              \
            fprintf(stderr, "FAILED: %s\n", result);                    \
        } else {                                                        \
            fprintf(stderr, "ALL TESTS PASSED\n");                      \
        }                                                               \
        fprintf(stderr, "Tests run: %d\n", tests_run);                  \
        exit(result != 0);                                              \
    }

int tests_run;

void (*setup_func)(void) = NULL;
void (*teardown_func)(void) = NULL;

#endif /* MINUNIT_H */
