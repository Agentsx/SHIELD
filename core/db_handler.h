#ifndef __DB_HANDLER_H__
#define __DB_HANDLER_H__

#include "include/tbl.h"
#include "utils/hash.h"
#include "utils/array.h"
#include "db/sqlite3/sqlite3.h"

#define DB_NAME "SHIELD.DB"

#define RECV   0
#define SEND   1

int db_conn_init(sqlite3 **conn);
int get_trade_date(sqlite3 *conn, char *date);
int get_user(sqlite3 *conn, const char *user_name, tbl_user_t *user);
int get_max_recv_no(sqlite3 *conn, const char *trade_date, long long *max_recv_no);
int get_send_trade_info_trans_no_greater_than(sqlite3 *conn, long long begin_recv_no, array_t *a);
int get_trade_list(sqlite3 *conn, hash_t *h,const char *trade_date);

#endif
