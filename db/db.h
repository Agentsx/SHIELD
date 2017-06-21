#ifndef __DB_H__
#define __DB_H__

#include "utils/array.h"
#include "include/sqlite3.h"

int db_init(const char *filename, sqlite3 **conn);
int db_exec_dql(sqlite3 *conn, const char *sql, char **err_msg, array_t *a);
int db_exec_dml(sqlite3 *conn, const char *sql, char **err_msg);
int db_close(sqlite3 *conn);

#endif
