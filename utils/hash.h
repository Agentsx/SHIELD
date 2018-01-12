#ifndef __IHASH_H__
#define __IHASH_H__

#include <stdlib.h>
#include "map.h"

#define DEBUG

typedef struct hash_s {
	map_t *map;
} hash_t;

hash_t *hash_init(int type);
hash_t *hash_init_with_cap(int type, size_t capacity);

void hash_destroy(hash_t *h);
int  hash_insert(hash_t *h, void *item);
int  hash_find(hash_t *h, void *item);
int  hash_remove(hash_t *h, void *item);
size_t  hash_count(hash_t *h);

#endif
