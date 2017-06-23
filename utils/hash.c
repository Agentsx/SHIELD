#include "hash.h"
#include "utils.h"
#include <string.h>

static int __ll_match(const void *a, const void *b)
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

unsigned int __hash(unsigned int (*hash)(const void *), void *item)
{
    if (item == NULL) 
        return 0;

    unsigned int h = hash(item);
    return h ^ (h >> 16);
}

static int __table_size_for(int cap) {
    int n = cap - 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return (n < 0) ? 1 : (n >= HASH_MAXIMUM_CAPACITY) ? HASH_MAXIMUM_CAPACITY : n + 1;
}

static void __list_destroy(hash_item_t *l, void (*destroy)(void *a))
{
    hash_item_t *p1, *p2;
    p1 = l;
    while (p1) {
        p2 = p1->next;
        destroy(p1);
        p1 = p2; 
    }
}

static void __buckets_destroy(hash_t *h, hash_item_t **b, size_t thr)
{
	size_t i;

    for (i = 0; i < thr; ++i) {
        if (b[i] != NULL) 
            __list_destroy(b[i], h->destroy);
    }

    free(b);
}

static int __get_pos(hash_t *h, void *item)
{
    unsigned int hash = __hash(h->hash, item);
    return hash & (h->threshold - 1);
}


hash_item_t *__find(hash_item_t *head, void *item, int (*match)(const void *a, const void *b))
{
    hash_item_t *p = head;
    while (p) {
        if (match(item, p->item))
            return p;
        p = p->next;
    }
    return NULL;
}

static void __resize(hash_t *h)
{
    if (h->threshold >= HASH_MAXIMUM_CAPACITY)
        return;

    size_t old_threshold = h->threshold;
    size_t new_threshold = h->threshold << 1;

    hash_item_t **new_buckets = (hash_item_t **)calloc(new_threshold, sizeof(hash_item_t *));

    size_t i;
    hash_item_t *p;
    for (i = 0; i < old_threshold; ++i) {
        if ((p = h->buckets[i]) != NULL) {

            if (p->next == NULL) {
                new_buckets[p->hash & (new_threshold - 1)] = p;
            } else {
                hash_item_t *next;
                hash_item_t *low_head = NULL, *low_tail = NULL;
                hash_item_t *high_head = NULL, *high_tail = NULL;

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
    free(h->buckets);
    h->buckets = new_buckets;
    h->threshold = new_threshold;
}

hash_t *hash_init(int type,
                  unsigned int hash(const void *a),
                  int (*match)(const void *a, const void *b),
                  void (*destroy)(void *a))
{
	return hash_init_with_cap(type, HASH_DEFAULT_INITIAL_CAPACITY, hash, match, destroy);
}

hash_t *hash_init_with_cap(int type,
                           size_t capacity,
                           unsigned int hash(const void *a),
                           int (*match)(const void *a, const void *b),
                           void (*destroy)(void *a))
{
	if (capacity < 0)
		return NULL;
	
	hash_t *h = NULL;
    if ((h = calloc(1, sizeof(hash_t))) == NULL)
        return NULL;

    switch (type) {
    case L:
    case LL:
	    h->hash = hash ? hash : __ll_hash;
        h->match = match ? match : __ll_match;
        h->destroy = free;
        break;
    case INT:
	    h->hash = hash ? hash : __int_hash;
        h->match = match ? match : __ll_match;
        h->destroy = free;
        break;
    case STR:
	    h->hash = hash ? hash : __str_hash;
        h->match = match ? match : __str_match;
        h->destroy = free;
        break;
    case POINTER:
        if (hash && match && destroy) {
            h->hash = hash;
            h->match = match;
            h->destroy = destroy;
        } else {
            free(h); 
            return NULL;
        }
        break;
    default:
        free(h);
        return NULL;
    }

    h->type = type;
    h->init_capacity = capacity > HASH_MAXIMUM_CAPACITY ? HASH_MAXIMUM_CAPACITY : capacity;
	h->threshold = __table_size_for(h->init_capacity);
    h->size = 0;
	h->buckets = (hash_item_t **)calloc(h->threshold, sizeof(hash_item_t *));

	return h;
}

void hash_destroy(hash_t *h)
{
	__buckets_destroy(h, h->buckets, h->threshold);

    free(h);
}

void *__calloc_for(int type, void *k)
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


int  hash_insert(hash_t *h, void *item)
{
    unsigned int pos = __get_pos(h, item);

    if (__find(h->buckets[pos], item, h->match) != NULL)
        return -1;

    hash_item_t *hp = calloc(1, sizeof(hash_item_t));
    void *v = __calloc_for(h->type, item);
    hp->item = v;
	hp->hash = __hash(h->hash, item);
    hp->next = h->buckets[pos];
    h->buckets[pos] = hp;

    if (++h->size > h->threshold)
        __resize(h);

    return 0;
}

int hash_find(hash_t *h, void *item, void **r)
{
    int pos = __get_pos(h, item);

    hash_item_t *hi = NULL;
    if ((hi = __find(h->buckets[pos], item, h->match)) == NULL)
        return -1;

    *r = hi->item;

    return 0;
}

size_t hash_count(hash_t *h)
{
    return h->size;
}

int hash_remove(hash_t *h, void *item)
{
    int pos = __get_pos(h, item);

    hash_item_t *hi = h->buckets[pos];
    hash_item_t *p1, *p2;
    p1 = p2 = hi;
    while (p1) {
        if (h->match(item, p1->item)) {
            if (p2 == hi) {
                hi = p2->next;
                h->destroy(p1->item);
                free(p1);
            } else {
                p2->next = p1->next; 
                h->destroy(p1->item);
                free(p1);
            }
            break;
        }
        p2 = p1;
        p1 = p1->next;
    }

    return 0;
}
