#include "hash.h"
#include "utils.h"
#include <string.h>

int val = 0;

hash_t *hash_init(int type)
{
	return hash_init_with_cap(type, MAP_DEFAULT_INITIAL_CAPACITY);
}

hash_t *hash_init_with_cap(int type, size_t capacity)
{
	map_t *map = map_init_with_cap(type, INT, capacity);
	if (map == NULL)
		return NULL;

	hash_t *h = calloc(1, sizeof(hash_t));
	h->map = map;
	return h;
}

void hash_destroy(hash_t *h)
{
	map_destroy(h->map);

    free(h);
}

int  hash_insert(hash_t *h, void *item)
{
	return map_put(h->map, item, (void *)&val);
}

int hash_find(hash_t *h, void *item)
{
	void *val;
	return map_get(h->map, item, &val);
}

size_t hash_count(hash_t *h)
{
    return map_count(h->map);
}

int hash_remove(hash_t *h, void *item)
{
	map_pair_t *mp;
	map_remove(h->map, item, &mp);
    return 0;
}
