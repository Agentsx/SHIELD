#ifndef __MAP_H__
#define __MAP_H__

#include <stdlib.h>

#define DEFAULT_INITIAL_CAPACITY (1 << 4)
#define MAXIMUM_CAPACITY         (1 << 30)

#define DEBUG
#include <stdio.h>

typedef struct map_pair_s {
    void               *key;
    void               *val;
	unsigned int       hash;
    struct map_pair_s *next;
} map_pair_t;

typedef struct map_s {
    int     key_type;
    int     val_type;
	size_t  init_capacity;
    size_t  threshold;
	size_t  size;
	int     (*match)(const void *a, const void *b);
	unsigned int  (*hash)(const void *s);
	map_pair_t   **buckets;
} map_t;

enum item_type{LL, INT, L, DOUBLE, STR, POINTER};

map_t *map_init(int key_type, int val_type);
map_t *map_init_with_cap(int key_type, int val_type, size_t capacity);

void map_destroy(map_t *h);
int  map_put(map_t *h, void *key, void *val);
int  map_get(map_t *h, void *key, void **val);
void **map_keys(map_t *h, size_t *size);
int  map_replace(map_t *h, void *key, void *val);
size_t  map_count(map_t *h);
void map_destroy_keys(void **keys);

#endif
