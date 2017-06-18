#include "db.h"
#include "utils/hash.h"
#include <stdio.h>

static int __callback(void *a, int argc, char **argv, char **col_name){
	int i;

    array_t *p = (array_t *)a;
    hash_t  *h = hash_init(STR, STR);
    if (h == NULL) {
        return -1;
    }

	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", col_name[i], argv[i] ? argv[i] : "NULL");
        hash_put(h, col_name[i], argv[i] ? argv[i] : "");
	}
    array_insert(p, (void *)h);
	printf("--\n");
	return 0;
}

int db_init(const char *filename, sqlite3 **conn)
{
	int ret;

	*conn = NULL;
	ret = sqlite3_open(filename, conn);
	if (ret) {
		if (*conn)
			sqlite3_close(*conn);
		return -1;
	}

	return 0;
}

int db_close(sqlite3 *conn)
{
	return sqlite3_close(conn) == SQLITE_OK ? 0 : -1;
}

int db_exec_dql(sqlite3 *conn, const char *sql, char **err_msg, array_t *a)
{
	return sqlite3_exec(conn, sql, __callback, (void *)a, err_msg);
}

int db_exec_dml(sqlite3 *conn, const char *sql, char **err_msg)
{
	return sqlite3_exec(conn, sql, NULL, NULL, err_msg) == SQLITE_OK ? 0 : -1;
}

int main()
{
	int ret;

	sqlite3 *conn;
	ret = db_init("test.db", &conn);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "open db error.\n"); 
        return ret;
    }
        
    char *sql = "select * from tbl1;";
    char *err_msg = NULL;

    array_t *a = array_init((array_item_destroy)hash_destroy);
   
    ret = db_exec_dql(conn, sql, &err_msg, a);
    if (ret == SQLITE_OK) {
        printf("a size [%ld].\n", array_count(a)); 
        int i;
        char *val;
        for (i = 0; i < array_count(a); ++i) {
            hash_t *p = (hash_t *)array_get(a, i); 

            ret = hash_get(p, "one", (void **)&val);
            if (!ret) {
                printf("%s--%s\n", "one", val); 
            }
            ret = hash_get(p, "two", (void *)&val);
            if (!ret) {
                printf("%s--%s\n", "two", val); 
            }
            printf("-----\n");
        }
    } else {
        printf("db exec dql error. [%s].\n", err_msg); 
    }

    array_destroy(a);

	return ret;
}
