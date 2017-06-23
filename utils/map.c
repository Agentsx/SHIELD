#include "map.h"
#include "utils.h"
#include <string.h>

static int __int_match(const void *a, const void *b)
{
    return *(long long *)a == *(long long *)b;
}

static int __str_match(const void *a, const void *b)
{
    return !strcmp((const char *)a, (const char *)b);
}

static unsigned int __str_hash(const void *key)
{
	const char     *ptr;
	unsigned int   val;

	val = 0;
	ptr = key;
	while (*ptr != '\0') {
		unsigned int tmp;
		val = (val << 4) + (*ptr);
		if ((tmp = (val & 0xf0000000))) {
			val = val ^(tmp >> 24);
			val = val ^ tmp;
		}
		ptr++;
	}
	
	return val;
}

static unsigned int __int_hash(const void *key)
{
	return *(int *)key;
}

static unsigned int __ll_hash(const void *key)
{
    long long k = *(long long *)key;
	return k ^ (k >> 32);
}

static int __table_size_for(int cap) {
    int n = cap - 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return (n < 0) ? 1 : (n >= MAP_MAXIMUM_CAPACITY) ? MAP_MAXIMUM_CAPACITY : n + 1;
}

void __map_pair_destroy(map_pair_t *p)
{
    free(p->key);
    free(p->val);
    free(p);
}

static void __list_destroy(map_pair_t *l)
{
    map_pair_t *p1, *p2;
    p1 = l;
    while (p1) {
        p2 = p1->next;
        __map_pair_destroy(p1);
        p1 = p2; 
    }
}

static void __buckets_destroy(map_pair_t **b, size_t thr)
{
	size_t i;

    for (i = 0; i < thr; ++i) {
        if (b[i] != NULL) 
            __list_destroy(b[i]);
    }

    free(b);
}

map_pair_t *__map_find(void *key, map_pair_t *head, int (*match)(const void *a, const void *b))
{
    map_pair_t *p = head;
    while (p) {
        if (match(key, p->key))
            return p;
        p = p->next;
    }
    return NULL;
}

static void __resize(map_t *m)
{
    if (m->threshold >= MAP_MAXIMUM_CAPACITY)
        return;

    size_t old_threshold = m->threshold;
    size_t new_threshold = m->threshold << 1;

    map_pair_t **new_buckets = (map_pair_t **)calloc(new_threshold, sizeof(map_pair_t *));

    size_t i;
    map_pair_t *p;
    for (i = 0; i < old_threshold; ++i) {
        if ((p = m->buckets[i]) != NULL) {

            if (p->next == NULL) {
                new_buckets[p->hash & (new_threshold - 1)] = p;
            } else {
                map_pair_t *next;
                map_pair_t *low_head = NULL, *low_tail = NULL;
                map_pair_t *high_head = NULL, *high_tail = NULL;

                do {
                    next = p->next;

                    if ((p->hash & old_threshold) == 0) {
                        if (low_tail == NULL)
                            low_head = p;
                        else
                            low_tail->next = p;
                        low_tail = p;
                    } else {
                        if (high_tail == NULL) 
                                high_head = p;
                        else
                            high_tail->next = p;
                        high_tail = p;
                    }
                } while ((p = next) != NULL);
                if (low_tail != NULL) {
                    low_tail->next = NULL; 
                    new_buckets[i] = low_head;
                }
                if (high_tail != NULL) {
                    high_tail->next = NULL; 
                    new_buckets[i + old_threshold] = high_head;
                }
            }
        }
    }
    free(m->buckets);
    m->buckets = new_buckets;
    m->threshold = new_threshold;
}

map_t *map_init(int key_type, int val_type)
{
	return map_init_with_cap(key_type, val_type, MAP_DEFAULT_INITIAL_CAPACITY);
}

map_t *map_init_with_cap(int key_type, int val_type, size_t capacity)
{
	if (capacity < 0)
		return NULL;
	
	map_t *m = NULL;
    if ((m = calloc(1, sizeof(map_t))) == NULL)
        return NULL;

    switch (key_type) {
    case L:
    case LL:
	    m->hash = __ll_hash;
        m->match = __int_match;
        break;
    case INT:
	    m->hash = __int_hash;
        m->match = __int_match;
        break;
    case STR:
	    m->hash = __str_hash;
        m->match = __str_match;
        break;
    default:
        free(m);
        return NULL;
    }

    switch (val_type) { /* check if the val_type in enum */
    case L:
    case LL:
    case INT:
    case STR:
    case DOUBLE:
    case POINTER:
        break;
    default:
        free(m);
        return NULL;
    }

    m->key_type = key_type;
    m->val_type = val_type;
    m->init_capacity = capacity > MAP_MAXIMUM_CAPACITY ? MAP_MAXIMUM_CAPACITY : capacity;
	m->threshold = __table_size_for(m->init_capacity);
    m->size = 0;
	m->buckets = (map_pair_t **)calloc(m->threshold, sizeof(map_pair_t *));

	return m;
}

void map_destroy(map_t *m)
{
	__buckets_destroy(m->buckets, m->threshold);

    free(m);
}

void *__map_calloc_for(int type, void *k)
{
    void *r;

    switch (type) {
    case INT: 
        r = calloc(1, sizeof(int));
        *(int *)r = *(int *)k;
        return r;
    case L: 
        r = calloc(1, sizeof(long));
        *(long *)r = *(long *)k;
        return r;
    case LL: 
        r = calloc(1, sizeof(long long));
        *(long long *)r = *(long long *)k;
        return r;
    case STR: 
        r = calloc(1, strlen((char *)k) + 1);
        strcpy((char *)r, (char *)k);
        return r;
    case DOUBLE: 
        r = calloc(1, sizeof(double));
        *(double *)r = *(double *)k;
        return r;
    case POINTER:
        return k;
    }
    return NULL;
}

unsigned int __map_hash(unsigned int hash)
{
    return hash ^ (hash >> 16);
}

int  map_put(map_t *m, void *key, void *val)
{
    unsigned int hash = key == NULL ? 0 : __map_hash(m->hash(key));
    unsigned int pos = hash & (m->threshold - 1);

    if (__map_find(key, m->buckets[pos], m->match) != NULL)
        return -1;

    map_pair_t *mp = calloc(1, sizeof(map_pair_t));
    void *newkey = __map_calloc_for(m->key_type, key);
    void *newval = __map_calloc_for(m->val_type, val);
    mp->key = newkey;
    mp->val = newval;
	mp->hash = hash;
    mp->next = m->buckets[pos];
    m->buckets[pos] = mp;

    if (++m->size > m->threshold)
        __resize(m);

    return 0;
}

int map_replace(map_t *m, void *key, void *val, void **oldval)
{
    unsigned int hash = key == NULL ? 0 : __map_hash(m->hash(key));
    unsigned int pos = hash & (m->threshold - 1);

    map_pair_t *p;
    if ((p = __map_find(key, m->buckets[pos], m->match)) == NULL) {
        map_put(m, key, val);
        *oldval = NULL;
    } else {
        *oldval = p->val;
        void *newval = __map_calloc_for(m->val_type, val);
        p->val = newval;
    }

    return 0;
}

int map_remove(map_t *m, void *key, map_pair_t **oldval)
{
    unsigned int hash = key == NULL ? 0 : __map_hash(m->hash(key));
    int pos = hash & (m->threshold - 1);

    map_pair_t *p = m->buckets[pos];
    map_pair_t *p1 = p;
    while (p) {
        if (m->match(key, p->key))
            break;
        p1 = p;
        p = p->next;
    }

    if (p != NULL) {
        p1->next = p->next;
        p->next = NULL;
    }

    *oldval = p;
    
    --m->size;
    return 0;
}

void **map_keys(map_t *m, size_t *size)
{
    *size = m->size;
    void **ks = (void **)calloc(*size, sizeof(void *));
    if (ks == NULL) {
        *size = 0;
        return NULL; 
    }

    int i, j = 0;
    map_pair_t *p = NULL;
    for (i = 0; i < m->threshold; ++i) {
         p = m->buckets[i];
         while (p) {
             ks[j++] = p->key;
             p = p->next;
         }
    }
    return ks;
}

void map_destroy_keys(void **keys)
{
    free(keys);
}

int map_get(map_t *m, void *key, void **val) /* it's a bad idea to use val pointer dirctly */
{
    unsigned int hash = key == NULL ? 0 : __map_hash(m->hash(key));
    int pos = hash & (m->threshold - 1);

    map_pair_t *mp = __map_find(key, m->buckets[pos], m->match);
    if (mp == NULL)
        return -1;
    *val = mp->val;

    return 0;
}

size_t map_count(map_t *m)
{
    return m->size;
}
