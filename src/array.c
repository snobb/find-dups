/*
 *  array.c
 *  Author: Alex Kozadaev (2015)
 */

#include <stdlib.h>

#include "util.h"
#include "array.h"

#define INITIAL_SIZE    32
#define ARRAY_FACTOR    2.0f

static void
array_resize(struct array *arr, float factor)
{
    size_t newallocsz = (int)(arr->allocated * factor);
    char **new = realloc(arr->values, newallocsz);

    CHECK_MEM(new);

    arr->values = new;
    arr->allocated = newallocsz;
}

struct array *
array_new(void)
{
    struct array *new = malloc(sizeof(*new));
    char **values;
    CHECK_MEM(new);

    new->allocated = INITIAL_SIZE;
    new->size = 0;
    values = calloc(sizeof(*(new->values)), new->allocated);
    CHECK_MEM(values);

    new->values = values;

    return new;
}

void
array_add(struct array *arr, const char *value)
{
    if (arr->size == arr->allocated) {
        array_resize(arr, ARRAY_FACTOR);
    }

    arr->values[arr->size++] = xstrndup(value, MAXPATH);
}

void
array_free(struct array *arr)
{
    free(arr->values);
    free(arr);
}

/* vim: set ts=4 sts=8 sw=4 smarttab et si tw=80 cino=t0l1(0k2s fo=crtocl */
