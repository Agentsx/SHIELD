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

int __check_client(add_vol_req_t *req)
{
	tbl_client_t client;
	int ret = get_client(g_core_data->db_conn, req->account_id, &client);
	if (ret) {
		printf("ERROR: [%s][%d] get client[%s] info error. .\n" , __FL__, acc_id);
		SET_RESULT(CLIENT_NOT_FOUND);
		return FALSE;
	}

	if (client.status == 0) {
		printf("ERROR: [%s][%d] client [%s] not in use.\n" , __FL__, acc_id);
		SET_RESULT(CLIENT_NOT_INUSE);
		return FALSE;
	}

	if (strcmp(req->pbu, client.pbu)) {
		printf("ERROR: [%s][%d] client [%s] not in use.\n" , __FL__, acc_id);
		SET_RESULT(PBU_ERROR);
		return FALSE;
	}

	return TRUE;
}

int __check_limit(add_vol_req_t *req)
{
	return TRUE;
}

static int __check_sge_instruction(const char *instruction_id)
{
	hash_t *h = hash_init(STR);
	int ret = get_sge_instrctions(g_core_data->db_conn, g_core_data->trade_date);
	if (ret) {
		printf("WARNING: [%s][%d] Add vol get sge instructions failed.\n", __FL__);
		return FALSE;
	}
	char *instr = NULL;
	ret = hash_find(h, (void *)instruction_id, (void **)&instr);
	if (ret)
		return FALSE;

	return TRUE;
}

static int __addvol_req_check(add_vol_req_t *req)
{
	int ret;
	ret = __check_sge_instruction(req->instruction_id);
	if (ret) {
		printf("WARNING: [%s][%d] Add vol check sge instruction failed.\n", __FL__);
		return FALSE;
	}

	tbl_trade_list_t *tl = NULL;
	if (map_get(g_core_data->trade_list, req->etf_code, (void **)&tbl)) {
		SET_RESULT(ETF_CODE_NOT_FOUND);
		return FALSE;
	}

	ret = __check_client(req);
	if (ret) {
		printf("WARNING: [%s][%d] Add vol check client failed.\n", __FL__);
		return FALSE;
	}

	ret = __check_limit(req);
	if (ret) {
		printf("WARNING: [%s][%d] Add vol check client failed.\n", __FL__);
		return FALSE;
	}
	return TRUE;
}

int add_vol_req_handler(shield_head_t *h)
{
	printf("TRACE: [%s][%d] add vol handler called.\n", __FL__);

	ADDVOL_CLEAR_RESULT();
	
	add_vol_req_t *add_vol_req = (add_vol_req_t *)(h + 1);

	if (add_vol_req->msg_head.trans_no <= g_core_data->recv_trans_no)
		return 0;

	int ret;
	ret = __addvol_req_check(add_vol_req);
	if (ret) {
		printf("WARNING: [%s][%d] Add vol check failed.\n", __FL__);
		goto AFTER;
	}

	SET_RESULT(TRADE_SUCCESS);

AFTER:
	{
	CALLOC_MSG(add_vol_rsp, h->fd, ADD_VOL_RSP);

	__package_addvol_head(&add_vol_rsp->msg_head);
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
	}
	return 0;
}
	

