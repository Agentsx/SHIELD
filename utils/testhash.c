#include "newhash.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct A {
    int a;  
    int b;
};

int main(int argc, char *argv[])
{
    /* Integer test */
    /* // begin
    hash_t *h = hash_init(LL);

    long long i;
    for (i = 0; i < 1000; ++i) {
        hash_insert(h, (void *)&i);
    }

    i = 900;
    hash_remove(h, &i);

    int ret; 
    for (i = 0; i < 1000; ++i) {
        ret = hash_find(h, (void *)&i);
        printf("find [%d] value [%lld].\n", ret, i);
    }

    hash_destroy(h);

    // end
	*/

    /* String test */
     // begin
    
    char s[53] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    hash_t *h = hash_init(STR);

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

    int  ret;
    for (i = 0; i < KEY_NUM; ++i) {
        ret = hash_find(h, key[i]);
        if (ret != 0)
            printf("Not found val from hash of key[%s] error.\n", key[i]);
        else
            printf("key[%s]--val[%s]\n", key[i], key[i]);
    }

    hash_destroy(h);

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
