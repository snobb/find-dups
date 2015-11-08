#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdlib.h>
#include "common.h"

/* dynamic array implementation */
struct array {
    char **values;
    size_t size;
    size_t allocated;
};

struct array *array_new(void);
void array_add(struct array *arr, const char *value);
void array_free(struct array *arr);

#endif /* __ARRAY_H__ */
