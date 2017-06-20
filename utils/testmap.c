#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
    /* Integer test */
    // begin
    map_t *h = map_init(LL, LL);

    long long key = 5;
    long long val = 10;

    int i;
    for (i = 0; i < 1000; ++i) {
        key = i;
        val = i + 2;
        map_put(h, &key, &val);
    }

    long long *gval = NULL;
    int ret;

    /*
    for (i = 0; i < 1000; ++i) {
        key = i;
        ret = map_get(h, &key, (void **)&gval);
        if (ret != 0)
            printf("Not found val from hash of key[%lld] error.\n", key);
        else
            printf("key[%lld]--val[%lld]\n", key, *gval);
        gval = NULL;
    }
    */

    // test map_keys
    size_t size = 0;
    void **keys = map_keys(h, &size);
    if (keys == NULL) {
        printf("get keys error.\n"); 
        return -1;
    }
    for (i = 0; i < size; ++i) {
        ret = map_get(h, keys[i], (void **)&gval);
        if (ret != 0)
            printf(".. Not found val from hash of key[%lld] error.\n", *(long long *)keys[i]);
        else
            printf(".. key[%lld]--val[%lld]\n", *(long long *)keys[i], *gval);
        gval = NULL;
    }

    map_destroy_keys(keys);
    // end

    map_destroy(h);

    // end

    /* String test */
    /* // begin
    
    char s[53] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    map_t *h = map_init_with_cap(STR, STR, 900);

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

        map_put(h, key[i], key[i]);
    }

    char *gval = NULL;
    int  ret;
    for (i = 0; i < KEY_NUM; ++i) {
        ret = map_get(h, key[i], (void **)&gval);
        if (ret != 0)
            printf("Not found val from hash of key[%s] error.\n", key[i]);
        else
            printf("key[%s]--val[%s]\n", key[i], gval);
        gval = NULL;
    }

    map_destroy(h);
    */ // end

    return 0;
}
