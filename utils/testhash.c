#include "ihash.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct A {
    int a;  
    int b;
};

static unsigned int __my_hash(const void *a)
{
    if (a == NULL)
        return 0;

    const struct A *ia = a;

    return (unsigned int)ia->a;
}

static void __my_destroy(void *a)
{
    free(a);
}

static int __my_match(const void *a, const void *b)
{
    const struct A *ia = a;
    const struct A *ib = b;

    if (ia == NULL && ib == NULL)
        return 1;

    if (ia == NULL || ib == NULL)
        return 0;
 
    return ia->a == ib->a && ia->b == ib->b;
}

int main(int argc, char *argv[])
{
    /* Integer test */
    // begin
    hash_t *h = hash_init(LL, NULL, NULL, NULL);

    long long i;
    for (i = 0; i < 1000; ++i) {
        hash_insert(h, (void *)&i);
    }

    i = 900;
    hash_remove(h, &i);

    long long *gval = NULL;
    int ret; 
    for (i = 0; i < 1000; ++i) {
        ret = hash_find(h, (void *)&i, (void **)&gval);
        printf("find [%d] value [%lld].\n", ret, ret == 0? *gval : -1);
    }

    hash_destroy(h);

    // end
    /* String test */
    /* // begin
    
    char s[53] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    hash_t *h = hash_init(STR, NULL, NULL, NULL);

    srandom(time(NULL));

    #define KEY_NUM 3000
    #define MAXSTRLEN 20

    int str_len;
    int i;
    char key[KEY_NUM][MAXSTRLEN + 2];
    for (i = 0; i < KEY_NUM; ++i) {
        str_len = random() % MAXSTRLEN + 1;
        int j;
        int index;
        memset(key[i], 0x00, sizeof(key[i]));
        for (j = 0; j < str_len; ++j) {
            index = random() % 52;
            key[i][j] = s[index];
        }

        hash_insert(h, key[i]);
    }

    char *gval = NULL;
    int  ret;
    for (i = 0; i < KEY_NUM; ++i) {
        ret = hash_find(h, key[i], (void **)&gval);
        if (ret != 0)
            printf("Not found val from hash of key[%s] error.\n", key[i]);
        else
            printf("key[%s]--val[%s]\n", key[i], gval);
        gval = NULL;
    }

    hash_destroy(h);
    */ // end

    /* POINTER TEST */
    /* // begin
    hash_t *h = hash_init(POINTER, __my_hash, __my_match, __my_destroy);

    srandom(time(NULL));

    #define NUM 2000

    int i;
    for (i = 0; i < NUM; ++i) {
        struct A *a = calloc(1, sizeof(struct A));
        a->a = i;
        a->b = i + 2;
        hash_insert(h, (void *)a);
    }

    struct A *gval = NULL;
    int  ret;
    struct A m;
    for (i = 0; i < NUM; ++i) {
        m.a = i;
        m.b = i + 2;
        ret = hash_find(h, &m, (void **)&gval);
        if (ret != 0)
            printf("Not found val from hash of a[%d] b[%d] error.\n", m.a, m.b);
        else
            printf("Found. a[%d] b[%d]\n", gval->a, gval->b);
        gval = NULL;
    }

    hash_destroy(h);
    */ // end

    return 0;
}
