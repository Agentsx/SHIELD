#include "db/db.h"
#include "db_handler.h"
#include "frame/frame.h"
#include "include/tbl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int db_conn_init(sqlite3 **conn)
{
    char *s = NULL;
    if (map_get(g_svr->cfg, "db_name", (void *)&s)) {
        printf("ERROR: [%s][%d] get db name from cfg error.\n", __FL__); 
        return -1;
    }
	return db_init(s, conn);
}

int get_trade_date(sqlite3 *conn, char *date)
{
	char *sql = "select max(f_trade_date) from t_trade_date;";

	array_t *a = array_init((array_item_destroy)map_destroy);

	int ret = 0;
	char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, a);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select trade date error. [%s].\n" , __FL__, err_msg);
		goto ERROR;
	}
	if (array_count(a) == 0)
		goto ERROR;

	map_t *h = (map_t *)array_get(a, 0);
	if (h == NULL)
		goto ERROR;

	char *trade_date = NULL;
	ret = map_get(h, "f_trade_date", (void **)&trade_date);
	if (ret != 0 || trade_date == NULL)
		goto ERROR;

	strncpy(date, trade_date, 8);
	array_destroy(a);
	return 0;

ERROR:
	array_destroy(a);
	return -1;
}

int get_user(sqlite3 *conn, const char *name, tbl_user_t *user)
{
    char *temp = "select f_username, f_password, f_status from t_user where f_username = '%s';";

    char sql[256];
    snprintf(sql, sizeof(sql), temp, name);

	array_t *a = array_init((array_item_destroy)map_destroy);
    int ret = 0;
    char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, a);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select user error. [%s].\n" , __FL__, err_msg);	
		goto ERROR;
	}
	if (array_count(a) == 0)
		goto ERROR;

	map_t *h = (map_t *)array_get(a, 0);
	if (h == NULL)
		goto ERROR;

    char *tmp = NULL;
    ret = map_get(h, "f_user_name", (void **)&tmp);
    if (ret)
        goto ERROR;
    strncpy(user->name, tmp, sizeof(user->name));

    ret = map_get(h, "f_password", (void **)&tmp);
    if (ret)
        goto ERROR;
    strncpy(user->password, tmp, sizeof(user->password));

    ret = map_get(h, "f_status", (void **)&tmp);
    if (ret)
        goto ERROR;
    user->status = atoi(tmp);

    array_destroy(a);
    return 0;

ERROR:
    array_destroy(a);
    return -1;
}

int get_client(sqlite3 *conn, const char *acc_no, tbl_client_t *client)
{
	char *temp = "select * from t_client where f_acc_no = '%s';";

    char sql[256];
    snprintf(sql, sizeof(sql), temp, acc_no);

	array_t *a = array_init((array_item_destroy)map_destroy);
    int ret = 0;
    char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, a);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select user error. [%s].\n" , __FL__, err_msg);	
		goto ERROR;
	}
	if (array_count(a) == 0)
		goto ERROR;

	map_t *h = (map_t *)array_get(a, 0);
	if (h == NULL)
		goto ERROR;

    char *tmp = NULL;
    ret = map_get(h, "f_acc_no", (void **)&tmp);
    if (ret)
        goto ERROR;
    strncpy(client->acc_no, tmp, sizeof(client->acc_no));

    ret = map_get(h, "f_pbu", (void **)&tmp);
    if (ret)
        goto ERROR;
    strncpy(client->pbu, tmp, sizeof(client->pbu));

    ret = map_get(h, "f_quantity", (void **)&tmp);
    if (ret)
        goto ERROR;
    client->quantity = atol(tmp);

	ret = map_get(h, "f_status", (void **)&tmp);
    if (ret)
        goto ERROR;
    client->status = atoi(tmp);

    array_destroy(a);
    return 0;

ERROR:
    array_destroy(a);
    return -1;
}

int get_max_recv_no(sqlite3 *conn, const char *trade_date, long long *max_recv_no)
{
    char *temp = "select max(f_trans_no) as f_trans_no from t_trade_info where f_trade_date = '%s' and f_recv_type = %d;";

    char sql[256];
    snprintf(sql, sizeof(sql), temp, trade_date, RECV);

	array_t *a = array_init((array_item_destroy)map_destroy);
    int ret = 0;
    char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, a);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select max trans_no error. [%s].\n" , __FL__, err_msg);	
		goto ERROR;
	}
	if (array_count(a) == 0)
		goto ERROR;

	map_t *h = (map_t *)array_get(a, 0);
	if (h == NULL)
		goto ERROR;

    char *tmp = NULL;
    ret = map_get(h, "f_trans_no", (void **)&tmp);
    if (ret)
        goto ERROR;

    *max_recv_no = atol(tmp);
    array_destroy(a);
    return 0;

ERROR:
    array_destroy(a);
    return -1;
}

int get_send_trade_info_trans_no_greater_than(sqlite3 *conn, long long begin_recv_no, array_t *a)
{
    char *temp = "select * from t_trade_info where f_trans_no > %lld and f_recv_type = %d;";
    char sql[256];
    snprintf(sql, sizeof(sql), temp, begin_recv_no, SEND);

	array_t *ia = array_init((array_item_destroy)map_destroy);
    int ret = 0;
    char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, ia);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select trade info error. [%s].\n" , __FL__, err_msg);	
		goto ERROR;
	}
	if (array_count(ia) == 0)
		goto ERROR;

    int i;
    map_t *h = NULL;
    tbl_trade_info_t  *trade_info = NULL;
    char *tmp = NULL;
    for (i = 0; i < array_count(a); ++i) {
	    h = (map_t *)array_get(ia, i);
        trade_info = calloc(1, sizeof(tbl_trade_info_t));
        map_get(h, "f_trade_date", (void **)&tmp);
        strncpy(trade_info->trade_date, tmp, sizeof(trade_info->trade_date));
        map_get(h, "f_sge_instruc", (void **)&tmp);
        strncpy(trade_info->sge_instruc, tmp, sizeof(trade_info->sge_instruc));
        map_get(h, "f_recv_type", (void **)&tmp);
        trade_info->recv_type = atoi(tmp);
        map_get(h, "f_trans_no", (void **)&tmp);
        trade_info->trans_no = atol(tmp);
        map_get(h, "f_msg_type", (void **)&tmp);
        trade_info->msg_type = atoi(tmp);
        map_get(h, "f_etf_code", (void **)&tmp);
        strncpy(trade_info->etf_code, tmp, sizeof(trade_info->etf_code));
        map_get(h, "f_client_acc", (void **)&tmp);
        strncpy(trade_info->client_acc, tmp, sizeof(trade_info->client_acc));
        map_get(h, "f_pbu", (void **)&tmp);
        strncpy(trade_info->pbu, tmp, sizeof(trade_info->pbu));
        map_get(h, "f_quantity", (void **)&tmp);
        trade_info->quantity = atol(tmp);
        map_get(h, "f_result_code", (void **)&tmp);
        strncpy(trade_info->result_code, tmp, sizeof(trade_info->result_code));
        map_get(h, "f_result_desc", (void **)&tmp);
        strncpy(trade_info->result_desc, tmp, sizeof(trade_info->result_desc));
        array_insert(a, (void *)trade_info);
    }

    array_destroy(a);
    return 0;

ERROR:
    array_destroy(ia);
    return -1;
}

int get_trade_time(sqlite3 *conn,array_t *a)
{
	char *sql = "select * from t_trade_time;";
	array_t *ia = array_init((array_item_destroy)map_destroy);
    int ret = 0;
    char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, a);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select trade time error. [%s].\n" , __FL__, err_msg);	
		goto ERROR;
	}
	if (array_count(a) == 0)
		goto ERROR;

    int i;
	map_t *h=NULL;
	tbl_trade_time_t  *trade_time = NULL;
	char *tmp = NULL;
	for (i = 0; i < array_count(ia); ++i) {
			trade_time = calloc(1, sizeof(tbl_trade_time_t));
			h = array_get(ia, i);
			map_get(h, "f_seq", (void **)&tmp);
			trade_time->seq = atoi(tmp);
			map_get(h, "f_start_time", (void **)&tmp);
			strncpy(trade_time->start_time, tmp, sizeof(trade_time->start_time));
			map_get(h, "f_end_time", (void **)&tmp);
			strncpy(trade_time->end_time, tmp, sizeof(trade_time->end_time));
			array_insert(a, (void *)trade_time);
		}

    array_destroy(a);
    return 0;

ERROR:
    array_destroy(ia);
    return -1;
}

int get_trade_list(sqlite3 *conn, map_t *h, const char *trade_date)
{
    char *temp = "select * from t_trade_list where f_trade_date = '%s';";
    char sql[256];
    snprintf(sql, sizeof(sql), temp, trade_date);

	array_t *a = array_init((array_item_destroy)map_destroy);
    int ret = 0;
    char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, a);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select trade info error. [%s].\n" , __FL__, err_msg);	
		goto ERROR;
	}
	if (array_count(a) == 0)
		goto ERROR;

    int i;
    map_t *ih = NULL;
    char *tmp = NULL;
    tbl_trade_list_t  *tl = NULL;
    for (i = 0; i < array_count(a); ++i) {
        tl = calloc(1, sizeof(tbl_trade_list_t));
        ih = array_get(a, i);
        map_get(ih, "f_etf_code", (void **)&tmp);
        strncpy(tl->etf_code, tmp, sizeof(tl->etf_code));
        map_get(ih, "f_apply_limit", (void **)&tmp);
        tl->apply_limit = atol(tmp);
        map_get(ih, "f_redemption_limit", (void **)&tmp);
        tl->redemption_limit = atol(tmp);
        map_put(h, tl->etf_code, tl);
    }

    array_destroy(a);
    return 0;

ERROR:
    array_destroy(a);
    return -1;
}

int get_sge_instrctions(sqlite3 *conn, const char *trade_date, hash_t *h)
{
	char *temp = "select f_sge_instruc from t_trade_info where f_trade_date = '%s';";
	char sql[256];
	snprintf(sql, sizeof(sql), temp, trade_date);

	array_t *ia = array_init((array_item_destroy)map_destroy);
	int ret = 0;
	char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, ia);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select trade info error. [%s].\n" , __FL__, err_msg);	
		goto ERROR;
	}

	int i;
	map_t *h = NULL;
	tbl_trade_info_t  *trade_info = NULL;
	char *tmp = NULL;
	for (i = 0; i < array_count(ia); ++i) {
		h = (map_t *)array_get(ia, i);
		map_get(h, "f_sge_instruc", (void **)&tmp);
		hash_insert(h, (void *)trade_info);
	}

    array_destroy(ia);
    return 0;
	
ERROR:
	array_destroy(ia);
	return -1;
}

int get_trade_vol(sqlite3 *conn, const char *trade_date, tbl_trade_vol_t *trade_vol)
{
	char *temp = "select * from t_trade_vol where f_trade_date = '%s' and f_etf_code = '%s';";
	char sql[256];
	snprintf(sql, sizeof(sql), temp, trade_date, etf_code);

	array_t *a = array_init((array_item_destroy)map_destroy);

	int ret = 0;
	char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, a);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select  from t_trade_list error. [%s].\n" , __FL__, err_msg);
		goto ERROR;
	}
	if (array_count(a) == 0)
		goto ERROR;

	map_t *h = (map_t *)array_get(a, 0);
	if (h == NULL)
		goto ERROR;

	char *tmp = NULL;
	ret = map_get(h, "f_apply_limit", (void **)&tmp);
	if (ret != 0 || tmp == NULL)
		goto ERROR;

	max_apply = atol(tmp);
	return 0;

ERROR:
	array_destroy(a);
	return -1;
}

int insert_trade_info(sqlite3 *conn, const tbl_trade_info_t *trade_info)
{
	char *temp = "insert into t_trade_info values('%s', '%s', %d, %d, %d, '%s', '%s', '%s', '%lld', '%s', '%s');"
	char sql[256];
	snprintf(sql, sizeof(sql), temp, trade_info->trade_date, \
	                                 trade_info->sge_instruc, \
									 trade_info->recv_type, \
									 trade_info->trans_no, \
									 trade_info->msg_type, \
									 trade_info->etf_code, \
									 trade_info->client_acc, \
									 trade_info->pbu, \
									 trade_info->quantity, \
									 trade_info->result_code, \
									 trade_info->result_desc);

	char *err_msg;
	if (db_exec_dml(conn, sql, &err_msg)) {
		printf("ERROR: [%s][%d] insert into t_trade_info error. [%s].\n" , __FL__, err_msg);
		return -1;
	}

	return 0;
}

int update_trade_vol(sqlite3 *conn, const char *trade_date, const char *etf_code, long long apply, long long redemption)
{
	char *temp = "update t_trade_vol set f_apply = f_apply + %lld, f_redemption = f_redemption + %lld " \
	             "where f_trade_date = '%s' and f_etf_code = '%s';";
	char sql[256];
	snprintf(sql, sizeof(sql), temp, apply, redemption);

	char *err_msg;
	if (db_exec_dml(conn, sql, &err_msg)) {
		printf("ERROR: [%s][%d] update t_trade_vol error. [%s].\n" , __FL__, err_msg);
		return -1;
	}
}

int update_client_quantity(sqlite3 *conn, const char *account_no, const char *pbu, long long quantity, int  status)
{
	char *temp = "update t_client set  f_pbu = %s ,f_quantity=%ld,f_status=%d" \
	             "where f_acc_no = '%s' ;";
	char sql[256];
	snprintf(sql, sizeof(sql), temp, pbu, quantity,status);

	char *err_msg;
	if (db_exec_dml(conn, sql, &err_msg)) {
		printf("ERROR: [%s][%d] update t_client error. [%s].\n" , __FL__, err_msg);
		return -1;
	}
}
