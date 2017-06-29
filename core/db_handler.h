#ifndef __DB_HANDLER_H__
#define __DB_HANDLER_H__

#include "db/sqlite3/sqlite3.h"

#define DB_NAME "SHIELD.DB"

int db_conn_init(sqlite3 **conn);
int get_trade_date(sqlite3 *conn, char *date);

#endif
