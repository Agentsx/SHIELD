#include "db/db.h"
#include "include/tbl.h"
#include "utils/log.h"
#include "utils/array.h"
#include "utils/map.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define __FL__ __FILE__, __LINE__

int get_trade_info_of_trade_date(sqlite3 *conn, const char *trade_date, array_t *a)
{
    char *temp = "select * from t_trade_info where f_trade_date = %s;";
    char sql[256];
    snprintf(sql, sizeof(sql), temp, trade_date);

	array_t *ia = array_init((array_item_destroy)map_destroy);
    int ret = 0;
    char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, ia);
	if (ret != 0) {
		log_error("select trade info error. [%s].", err_msg);
		goto ERROR;
	}
	if (array_count(ia) == 0)
		goto ERROR;

    int i;
    map_t *h = NULL;
    tbl_trade_info_t  *trade_info = NULL;
    char *tmp = NULL;
    for (i = 0; i < array_count(ia); ++i) {
	    h = (map_t *)array_get(ia, i);
        trade_info = calloc(1, sizeof(tbl_trade_info_t));
        map_get(h, "f_sge_instruc", (void **)&tmp);
        strncpy(trade_info->sge_instruc, tmp, sizeof(trade_info->sge_instruc));
		map_get(h, "f_recv_type", (void **)&tmp);
        trade_info->recv_type = atoi(tmp);
        map_get(h, "f_msg_type", (void **)&tmp);
        trade_info->msg_type = atoi(tmp);
        map_get(h, "f_etf_code", (void **)&tmp);
        strncpy(trade_info->etf_code, tmp,sizeof(trade_info->etf_code));
        map_get(h, "f_client_acc", (void **)&tmp);
        strncpy(trade_info->client_acc, tmp, sizeof(trade_info->client_acc));
        map_get(h, "f_pbu", (void **)&tmp);
        strncpy(trade_info->pbu, tmp, sizeof(trade_info->pbu));
        map_get(h, "f_quantity", (void **)&tmp);
        trade_info->quantity = atol(tmp);
        map_get(h, "f_result_code", (void **)&tmp);
        strncpy(trade_info->result_code, tmp, sizeof(trade_info->result_code));
			
        array_insert(a, (void *)trade_info);
    }
   
    return 0;

ERROR:
    array_destroy(ia);
    return -1;
}

int get_trade_date(sqlite3 *conn, char *date)
{
	char *sql = "select max(f_trade_date) as f_trade_date from t_trade_date;";
	array_t *a = array_init((array_item_destroy)map_destroy);
	int ret = 0;
	char *err_msg = NULL;
	
	ret = db_exec_dql(conn, sql, &err_msg, a);
	if (ret != 0) {
		printf("select trade date error. [%s]." , err_msg);
		goto ERROR;
	}
	
	if (array_count(a) == 0)
		goto ERROR;

	map_t *m = (map_t *)array_get(a, 0);
	if (m == NULL)
		goto ERROR;

	char *trade_date = NULL;
	ret = map_get(m, "f_trade_date", (void **)&trade_date);
	if (ret != 0 || trade_date == NULL)
		goto ERROR;

	strncpy(date, trade_date, 8);
	array_destroy(a);
	return 0;

ERROR:
	array_destroy(a);
	return -1;
}

int main()
{
	char datafile[20];
	char trade_date[16];
	sqlite3 *db_conn=NULL;
	
	int ret=db_init("db/SHIELD.DB",&db_conn);
	if (ret) {
        printf("db init error!\n");
        return -1;
    }
	/*
	ret=log_init("conf/log.conf");
	if (ret) {
        printf("log init error!\n");
        return -1;
    }
	*/
	ret=get_trade_date(db_conn, trade_date);
	if (ret) {
        printf("no trade date found\n");
        return -1;
    }
	
	sprintf(datafile,"clearing/ssjg0%s001.txt", trade_date);
	FILE *fd=fopen(datafile,"w+");
	if(fd==NULL) 
		return -1;
	
	array_t *a = array_init(NULL);
	
	ret = get_trade_info_of_trade_date(db_conn,trade_date,a);
    if (ret) {
        printf("no trade info found\n");
        return -1;
    }

	int i;
	tbl_trade_info_t *trade_info;
	char trade_type[4];

    for (i = 0; i < array_count(a); ++i) {
    	trade_info = (tbl_trade_info_t *)array_get(a, i); 

		if( (trade_info->recv_type == 1) && (strcmp(trade_info->result_code,"00002")) && (strcmp(trade_info->result_code,"1"))) {
			
			if(trade_info->msg_type == 10)
				strncpy(trade_type,"022",sizeof(trade_type));
			else
				strncpy(trade_type,"024",sizeof(trade_type));
			
			fprintf(fd, "%16s|%6s|%10s|%6s|%16lld|%3s|%5s\n", trade_info->sge_instruc,\
													trade_info->etf_code,\
													trade_info->client_acc,\
													trade_info->pbu,\
													trade_info->quantity,\
													trade_type,\
													trade_info->result_code);
			
			}
        }
	array_destroy(a);	
	db_close(db_conn);
    fclose(fd);
	return 0;
}