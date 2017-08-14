#include "array.h"
#include <string.h>

void *array_get(array_t *a, size_t i)
{
    if (i > (a->size - 1))
        return NULL;

    return a->data[i];
}

static int __array_resize(array_t *a)
{
    if (a->capacity >= ARRAY_MAX_CAP || (a->capacity << 1) > ARRAY_MAX_CAP)
        return -1;

    a->capacity <<= 1;
    void **new = (void **)calloc(a->capacity, sizeof(void *));
    memcpy(new, a->data, a->size * sizeof(void *));
    free(a->data);
    a->data = new;
    return 0;
}

int array_insert(array_t *a, void *d)
{
    a->data[a->size++] = d;
    if (a->size >= a->capacity)
        return __array_resize(a);

    return 0;
}

size_t array_count(array_t *a)
{
    return a->size;
}

array_t *array_init(array_item_destroy destroy)
{
    array_t *a = calloc(1, sizeof(array_t));
    a->capacity = ARRAY_DEFAULT_CAP;
    a->data = (void **)calloc(ARRAY_DEFAULT_CAP, sizeof(void *));
    a->destroy = destroy ? destroy : free;
    return a;
}

void array_destroy(array_t *a)
{
    int i;
    for (i = 0; i < a->size; ++i) {
        if (a->data[i] != NULL)
            a->destroy(a->data[i]);
    }

    free(a->data);
    free(a);
}
