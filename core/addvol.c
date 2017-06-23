#include "middle/middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "db/db.h"
#include "db_handler.h"
#include "frame/frame.h"
#include "include/tbl.h"
#include "utils/utils.h"
#include "utils/array.h"
#include "utils/hash.h"
#include "utils/log.h"
#include "err.h"
#include "core.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static int __package_addvol_rsp_head(msg_head_t *h)
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

	return TRUE;
}

int __addvol_check_client(add_vol_req_t *req)
{
	tbl_client_t client;
	int ret = get_client(g_core_data->db_conn, req->account_id, &client);
	if (ret) {
		log_error("get client[%s] info error. ." , req->account_id);
		SET_RESULT(CLIENT_NOT_FOUND);
		return FALSE;
	}

	if (client.status == 0) {
		log_error("client [%s] not in use." , req->account_id);
		SET_RESULT(CLIENT_NOT_INUSE);
		return FALSE;
	}

	if (strcmp(req->PBU, client.pbu)) {
		log_error("client [%s] not in use." , req->account_id);
		SET_RESULT(PBU_ERROR);
		return FALSE;
	}

	return TRUE;
}

int __addvol_check_limit(add_vol_req_t *req)
{
	tbl_trade_vol_t trade_vol;
	int ret = get_trade_vol(g_core_data->db_conn, g_core_data->trade_date, req->instrument_id, &trade_vol);
	if (ret) {
		log_warn("Add vol get etf[%s] trade vol failed.", req->instrument_id);
		SET_RESULT(SO_BAD);
		return FALSE;
	}

    tbl_trade_list_t *tl = NULL;
    ret = map_get(g_core_data->trade_list, req->instrument_id, (void **)&tl);
    if (ret) {
		log_warn("Add vol get etf[%s] trade list.", req->instrument_id);
		SET_RESULT(SO_BAD);
        return FALSE ;
    }

	if (tl->apply_limit < req->quantity + trade_vol.apply) {
		log_warn("Add vol get etf[%s] trade vol failed.", req->instrument_id);
		SET_RESULT(BEYOND_APPLY_LIMIT);
		return FALSE;
	}

	return TRUE;
}

static int __addvol_check_sge_instruction(const char *instruction_id)
{
	hash_t *h = hash_init(STR, NULL, NULL, NULL);
	int ret = get_sge_instrctions(g_core_data->db_conn, g_core_data->trade_date, h);
	if (ret) {
		log_warn("Add vol get sge instructions failed.");
        SET_RESULT(SO_BAD);
        goto ERROR;
	}

	char *instr = NULL;
	ret = hash_find(h, (void *)instruction_id, (void **)&instr);
	if (ret) {
		log_warn("Add vol sge instructions already handled.");
        SET_RESULT(INSTRUCTION_HANDLED);
        goto ERROR;
    }

    hash_destroy(h);
	return TRUE;

ERROR:
    hash_destroy(h);
    return FALSE;
}

static int __addvol_check_trade_time()
{
	char cur_time[16];
	time_t timep;  
	struct tm *p;  
	time(&timep);  
	p =localtime(&timep);
	sprintf(cur_time,"%d:%d", p->tm_hour, p->tm_min);
	
	array_t *a = array_init(NULL);
	int ret = 0;
	ret = get_trade_time(g_core_data->db_conn , a);
	if (ret) {
	    log_error("failed to find trade time !");
	    return -1;
	}

	int i;
	tbl_trade_time_t *trade_time = NULL;
	char start_time[2][16];
	char end_time[2][16];
	for (i = 0; i < array_count(a); ++i) {
	    trade_time = (tbl_trade_time_t *)array_get(a, i); 
		strncpy(start_time[i], trade_time->start_time, sizeof(trade_time->start_time));
		strncpy(end_time[i], trade_time->end_time, sizeof(trade_time->end_time));
		if((strcmp(cur_time, start_time[i]) >= 0) && (strcmp(cur_time, end_time[i]) <= 0) )
			return TRUE;	
	}

	log_error("it's not trade time now!");
	SET_RESULT(TRADE_TIME_ERR);
	array_destroy(a);
	return FALSE;

}

static int __addvol_req_check(add_vol_req_t *req)
{
	int ret;

	ret=__addvol_check_trade_time();
	if (ret) {
		log_warn("Add vol check trade time failed.");
		return FALSE;
	}
	
	ret = __addvol_check_sge_instruction(req->instruction_id);
	if (ret) {
		log_warn("Add vol check sge instruction failed.");
		return FALSE;
	}

	tbl_trade_list_t *tl = NULL;
	if (map_get(g_core_data->trade_list, req->instrument_id, (void **)&tl)) {
		SET_RESULT(ETF_CODE_NOT_FOUND);
		return FALSE;
	}

	ret = __addvol_check_client(req);
	if (ret) {
		log_warn("Add vol check client failed.");
		return FALSE;
	}

	ret = __addvol_check_limit(req);
	if (ret) {
		log_warn("Add vol check client failed.");
		return FALSE;
	}
	return TRUE;
}

int __addvol_insert_info(add_vol_req_t *req, add_vol_rsp_t *rsp)
{
	tbl_trade_info_t trade_info;
	memset(&trade_info, 0, sizeof(tbl_trade_info_t));

	/* req */
	STRNCPY(trade_info.trade_date, g_core_data->trade_date);
	STRNCPY(trade_info.sge_instruc, req->instruction_id);
	trade_info.recv_type = RECV;
	trade_info.trans_no = req->msg_head.trans_no;
	trade_info.msg_type = ADD_VOL_REQ;
	STRNCPY(trade_info.etf_code, req->instrument_id);
	STRNCPY(trade_info.client_acc, req->account_id);
	STRNCPY(trade_info.pbu, req->PBU);
	trade_info.quantity = req->quantity;

	insert_trade_info(g_core_data->db_conn, &trade_info);

	/* rsp */
	trade_info.recv_type = SEND;
	STRNCPY(trade_info.result_code, rsp->processing_result);
	STRNCPY(trade_info.result_desc, rsp->description);
	trade_info.trans_no = rsp->msg_head.trans_no;
	trade_info.msg_type = ADD_VOL_RSP;

	insert_trade_info(g_core_data->db_conn, &trade_info);

	return TRUE;
}

int __addvol_update_trade_vol(const char *etf_code, long long quantity)
{
	return update_trade_vol(g_core_data->db_conn, g_core_data->trade_date, etf_code, quantity, 0);
}
int __addvol_update_client_quantity(const char *account_id,const char *pbu,long long quantity)
{
	tbl_client_t client;
	int ret = get_client(g_core_data->db_conn,account_id, &client);
	if (ret) {
		return FALSE;
	}
	return update_client_quantity(g_core_data->db_conn, account_id, pbu, quantity, client.status);
}

static int __addvol_update_db(add_vol_req_t *req, add_vol_rsp_t *rsp)
{
	__addvol_insert_info(req, rsp);

	if (strcmp(rsp->processing_result, TRADE_OK) == 0) {
		__addvol_update_trade_vol(req->instrument_id, req->quantity);
		
		__addvol_update_client_quantity(req->account_id,req->PBU,req->quantity);
	}

	return TRUE;
}

int add_vol_req_handler(shield_head_t *h)
{
	log_notice("add vol handler called.");

	CLEAR_RESULT();
	
	add_vol_req_t *add_vol_req = (add_vol_req_t *)(h + 1);

	int ret;
	ret = __addvol_req_check(add_vol_req);
	if (ret) {
		log_warn("Add vol check failed.");
		goto AFTER;
	}

	SET_RESULT(TRADE_SUCCESS);

AFTER:
	{
		CALLOC_MSG(add_vol_rsp, h->fd, ADD_VOL_RSP);

		__package_addvol_rsp_head(&add_vol_rsp->msg_head);

		STRNCPY(add_vol_rsp->processing_result, result_code);
		STRNCPY(add_vol_rsp->description, result_desc);
		STRNCPY(add_vol_rsp->org_instruction_id, add_vol_req->instruction_id);
		STRNCPY(add_vol_rsp->instrument_id, add_vol_req->instrument_id);
		STRNCPY(add_vol_rsp->PBU, add_vol_req->PBU);
		STRNCPY(add_vol_rsp->account_id, add_vol_req->account_id);
		add_vol_rsp->quantity = add_vol_req->quantity;

		__addvol_update_db(add_vol_req, add_vol_rsp);
		
		PUSH_MSG(add_vol_rsp);
	}
	return 0;
}
	

