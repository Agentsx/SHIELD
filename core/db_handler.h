#ifndef __DB_HANDLER_H__
#define __DB_HANDLER_H__

#include "include/tbl.h"
#include "utils/hash.h"
#include "utils/map.h"
#include "utils/array.h"
#include "include/sqlite3.h"

int db_conn_init(sqlite3 **conn);
int get_trade_date(sqlite3 *conn, char *date);
int get_user(sqlite3 *conn, const char *user_name, tbl_user_t *user);
int get_client(sqlite3 *conn, const char *acc_no, tbl_client_t *client);
int get_max_recv_no(sqlite3 *conn, const char *trade_date, long long *max_recv_no);
int get_send_trade_info_trans_no_greater_than(sqlite3 *conn, long long begin_recv_no, array_t *a);
int get_trade_time(sqlite3 *conn, array_t *a);
int get_trade_count(sqlite3 *conn, const char *trade_date, size_t *count);
int get_trade_list(sqlite3 *conn, map_t *h, const char *trade_date);
int get_sge_instrctions(sqlite3 *conn, const char *trade_date, hash_t *h);
int get_trade_vol(sqlite3 *conn, const char *trade_date, const char *etf_code, tbl_trade_vol_t *trade_vol);
int insert_trade_info(sqlite3 *conn, const tbl_trade_info_t *trade_info);
int update_trade_vol(sqlite3 *conn, const char *trade_date, const char *etf_code, long long apply, long long redemption);
int update_client_quantity(sqlite3 *conn, const char *account_no, long long quantity);
int get_trade_result(sqlite3 *conn, const char *trade_date, const char *org_instruction, tbl_trade_info_t *t);


#endif
