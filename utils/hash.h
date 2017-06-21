#ifndef __IHASH_H__
#define __IHASH_H__

#include <stdlib.h>

#define HASH_DEFAULT_INITIAL_CAPACITY 1 << 4
#define HASH_MAXIMUM_CAPACITY         1 << 30

#define DEBUG
#include <stdio.h>

typedef struct hash_item_s {
    void               *item;
	unsigned int       hash;
    struct hash_item_s *next;
} hash_item_t;

typedef struct hash_s {
    int           type;
	size_t        init_capacity;
    size_t        threshold;
	size_t        size;
	int           (*match)(const void *a, const void *b);
	void          (*destroy)(void *a);
	unsigned int  (*hash)(const void *s);
	hash_item_t   **buckets;
} hash_t;

hash_t *hash_init(int type,
                  unsigned int (*hash)(const void *a),
                  int (*match)(const void *a, const void *b),
                  void (*destroy)(void *a));
hash_t *hash_init_with_cap(int type,
                           size_t capacity,
                           unsigned int (*hash)(const void *a),
                           int (*match)(const void *a, const void *b),
                           void (*destroy)(void *a));

void hash_destroy(hash_t *h);
int  hash_insert(hash_t *h, void *item);
int  hash_find(hash_t *h, void *item, void **r);
int  hash_remove(hash_t *h, void *item);
size_t  hash_count(hash_t *h);

#endif
