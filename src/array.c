/*
 *  array.c
 *  Author: Alex Kozadaev (2015)
 */

#include <stdlib.h>

#include "util.h"
#include "array.h"

#define INITIAL_SIZE    32
#define ARRAY_FACTOR    2.0f

struct array *
array_new(void)
{
    struct array *new = malloc(sizeof(*new));
    CHECK_MEM(new);

    new->allocated = INITIAL_SIZE;
    new->size = 0;
    new->values = calloc(sizeof(*new->values), new->allocated);
    CHECK_MEM(new->values);

    return new;
}

void
array_add(struct array *arr, const char *value)
{
    char **new;
    if (arr->size >= arr->allocated) {
        arr->allocated = (int)(arr->allocated * ARRAY_FACTOR);
        new = realloc(arr->values, arr->allocated * sizeof(*arr->values));
        if (!new || new == arr->values) {
            die("error: realloc failed");
        }

        arr->values = new;
    }

    arr->values[arr->size++] = xstrndup(value, MAXPATH);
}

void
array_free(struct array *arr)
{
    for (int i = 0; i < arr->size; ++i) {
        free(arr->values[i]);
    }
    free(arr->values);
    free(arr);
}

