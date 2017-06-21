#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdlib.h>

#define ARRAY_DEFAULT_CAP 16
#define ARRAY_MAX_CAP     (1 << 30)

typedef void (*array_item_destroy)(void *);

typedef struct array_s {
    size_t capacity;
    size_t size;
    void   **data;
    array_item_destroy destroy;
} array_t;

array_t *array_init(array_item_destroy);
int     array_insert(array_t *a, void *d);
void    *array_get(array_t *a, size_t index);
size_t  array_count(array_t *a);
void    array_destroy(array_t *a);

#endif
