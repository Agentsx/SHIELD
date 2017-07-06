#include "core.h"
#include "middle/middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "db/db.h"
#include "db_handler.h"
#include "frame/frame.h"
#include "include/tbl.h"
#include "utils/array.h"
#include "core/err.h"


#include <stdio.h>
#include <string.h>


#define TRUE 0
#define FALSE 1

#define ADDVOL_OK  "00001"
#define ADDVOL_ERR "00000"

#define ADDVOL_CLEAR_RESULT() {memset(process_result, 0, sizeof(process_result));\
								memset(process_dscp, 0, sizeof(process_dscp));}

#define ADDVOL_CPY_RESULT_DESC(err) strncpy(process_dscp, err, sizeof(process_dscp))

char process_result[8];
char process_dscp[40];


static int __package_addvol_head(msg_head_t *h)
{
	h->msg_len = ADDVOL_RSP_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = ADDVOL_RSP_BODY_LEN; 
	h->rec_no = 1; 
	strncpy(h->msg_type, A302, sizeof(h->msg_type));
	h->trans_no = 0; 
	h->signature_flag = NONSIGNATURED; 
	h->encrypted = NONENCRYTED; 
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 
	return 0;
}

static int get_sge_instrction(sqlite3 *conn,char * sge_instrction,array_t *aa)
{
	char *temp = "select * from t_trade_info where f_sge_instruc = %s;";
	char sql[256];
	snprintf(sql, sizeof(sql), temp,sge_instrction );

	array_t *ia = array_init((array_item_destroy)map_destroy_keys);
	int ret = 0;
	char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, ia);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select trade info error. [%s].\n" , __FL__, err_msg);	
		goto ERROR;
	}
	if (array_count(ia) == 0) {
		array_destroy(aa);
		return FALSE;//未处理过
	}
	else {//已有记录
		int i;
		map_t *h = NULL;
		tbl_trade_info_t  *trade_info = NULL;
		char *tmp = NULL;
		for (i = 0; i < array_count(aa); ++i) {
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
		    array_insert(aa, (void *)trade_info);

			array_destroy(aa);
			return TRUE;
		}

	}
	
ERROR:
	array_destroy(aa);
	return -1;
}

long long __get_apply_limit(sqlite3 *conn,char * trade_date,char * etf_code)
{
	long long max_apply;
	char *temp = "select f_apply_limit from t_trade_list where f_trade_date = %s and f_etf_code=%s ;";
	char sql[256];
	snprintf(sql, sizeof(sql), temp,trade_date,etf_code );

	array_t *a = array_init((array_item_destroy)map_destroy_keys);

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

	char *tmp=NULL;
	ret = map_get(h, "f_apply_limit", (void **)&tmp);
	
	if (ret != 0 || tmp == NULL)
		goto ERROR;
	max_apply = atol(tmp);
	return max_apply;	
ERROR:
	array_destroy(a);
	return -1;
}

#define STRNCPY(a, b) strncpy(a, b, sizeof(a))

static int __send_addvol_rsp(shield_head_t *h, const tbl_trade_info_t *trade_info)
{
#define __package_trade_rsp_body(rsp) \
	do { \
		STRNCPY(rsp->processing_result, trade_info->result_code); \
		STRNCPY(rsp->description, trade_info->result_desc); \
		STRNCPY(rsp->org_instruction_id, trade_info->sge_instruc); \
		STRNCPY(rsp->instrument_id, trade_info->etf_code); \
		STRNCPY(rsp->account_id, trade_info->client_acc); \
		STRNCPY(rsp->PBU, trade_info->pbu); \
		rsp->quantity = trade_info->quantity; \
		} while (0)

	return 0;
}
int __insert_into_trade_info(sqlite3* conn,add_vol_req_t * req)
{
	char *temp = "insert into t_trade_info values(%s%s%d%d%d%s%s%s%ld%s%s) ;";
	char sql[256];
	char *err_msg = NULL;
	int ret=0;
	snprintf(sql, sizeof(sql), temp,g_core_data->trade_date,\
							req->instruction_id,\
							0,\
							req->msg_head.trans_no,\
							req->msg_head.msg_type,\
							req->instrument_id,\
							req->account_id,\
							req->PBU,\
							req->quantity,\
							process_result,\
							process_dscp);
	ret=db_exec_dml(conn, sql, &err_msg);
	if (ret != 0) {
		printf("ERROR: [%s][%d] insert into trade info error. [%s].\n" , __FL__, err_msg);	
		return -1;
	}
	return 0;
}

int add_vol_req_handler(shield_head_t *h)
{
	printf("TRACE: [%s][%d] add vol handler called.\n", __FL__);

	ADDVOL_CLEAR_RESULT();
	
	add_vol_req_t *add_vol_req = (add_vol_req_t *)(h + 1);

	CALLOC_MSG(add_vol_rsp, h->fd, ADD_VOL_RSP);

	__package_addvol_head(&add_vol_rsp->msg_head);

	array_t *a = array_init(NULL);
	int ret = get_sge_instrction(g_core_data->db_conn,add_vol_req->instruction_id,a);
	
	if (ret == FALSE) {
		//未处理过，先入库，组报文
		long long apply_limit=__get_apply_limit(g_core_data->db_conn,g_core_data->trade_date,add_vol_req->instrument_id);
		if(apply_limit == -1){
			strncpy(process_result , ADDVOL_ERR,sizeof(process_result));
			ADDVOL_CPY_RESULT_DESC(APPLY_LIMIT_NOT_FOUND);			
		}
		else{
			if(add_vol_req->quantity>apply_limit){
				strncpy(process_result , ADDVOL_ERR,sizeof(process_result));
				ADDVOL_CPY_RESULT_DESC(QUANTITY_ERROR);	
			}
			else{
				strncpy(process_result , ADDVOL_OK,sizeof(process_result));
			}
		}
		__insert_into_trade_info(g_core_data->db_conn,add_vol_req);
		strncpy(add_vol_rsp->processing_result, process_result, sizeof(add_vol_rsp->processing_result));
		strncpy(add_vol_rsp->description, process_dscp, sizeof(add_vol_rsp->description));
		strncpy(add_vol_rsp->org_instruction_id, add_vol_req->instruction_id, sizeof(add_vol_rsp->org_instruction_id));
		strncpy(add_vol_rsp->instrument_id, add_vol_req->instrument_id, sizeof(add_vol_rsp->instrument_id));
		strncpy(add_vol_rsp->PBU, add_vol_req->PBU, sizeof(add_vol_rsp->PBU));
		add_vol_rsp->quantity=add_vol_req->quantity;
	}
	
	else if (ret == TRUE){
		//处理过，返回第一次的处理结果，
		int i;
		tbl_trade_info_t *trade_info = NULL;
		for (i = 0; i < array_count(a); ++i) {
			trade_info = (tbl_trade_info_t *)array_get(a, i); 
			__send_addvol_rsp(h, trade_info);
		}
	}
	else{
		return -1;
	}
	array_destroy(a);
	PUSH_MSG(add_vol_rsp);	
	return 0;
}
	

