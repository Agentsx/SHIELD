#include "middle/middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "db/db.h"
#include "db_handler.h"
#include "frame/frame.h"
#include "include/tbl.h"
#include "utils/array.h"
#include "utils/hash.h"
#include "utils/utils.h"
#include "utils/log.h"
#include "err.h"
#include "core.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static int __package_cutvol_rsp_head(msg_head_t *h)
{
	h->msg_len = CUTVOL_RSP_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = CUTVOL_RSP_BODY_LEN; 
	h->rec_no = 1; 
	strncpy(h->msg_type, A304, sizeof(h->msg_type));
	h->trans_no = 0; 
	h->signature_flag = NONSIGNATURED; 
	h->encrypted = NONENCRYTED; 
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 

	return TRUE;
}

int __cutvol_check_client(cut_vol_req_t *req)
{
	tbl_client_t client;
	int ret = get_client(g_core_data->db_conn, req->account_id, &client);
	if (ret) {
		printf("ERROR: [%s][%d] get client[%s] info error. .\n" , __FL__, req->account_id);
		SET_RESULT(CLIENT_NOT_FOUND);
		return FALSE;
	}

	if (client.status == 0) {
		printf("ERROR: [%s][%d] client [%s] not in use.\n" , __FL__, req->account_id);
		SET_RESULT(CLIENT_NOT_INUSE);
		return FALSE;
	}

	if (strcmp(req->PBU, client.pbu)) {
		printf("ERROR: [%s][%d] client [%s] not in use.\n" , __FL__, req->account_id);
		SET_RESULT(PBU_ERROR);
		return FALSE;
	}

	return TRUE;
}

int __cutvol_check_limit(cut_vol_req_t *req)
{
	tbl_trade_vol_t trade_vol;
	int ret = get_trade_vol(g_core_data->db_conn, g_core_data->trade_date, req->instrument_id, &trade_vol);
	if (ret) {
		printf("WARNING: [%s][%d] cut vol get etf[%s] trade vol failed.\n", __FL__, req->instrument_id);
		SET_RESULT(SO_BAD);
		return FALSE;
	}

    tbl_trade_list_t *tl = NULL;
    ret = map_get(g_core_data->trade_list, req->instrument_id, (void **)&tl);
    if (ret) {
		printf("WARNING: [%s][%d] cut vol get etf[%s] trade list.\n", __FL__, req->instrument_id);
		SET_RESULT(SO_BAD);
        return FALSE ;
    }

	if (tl->redemption_limit < req->quantity + trade_vol.redemption) {
		printf("WARNING: [%s][%d] cut vol get etf[%s] trade vol failed.\n", __FL__, req->instrument_id);
		SET_RESULT(BEYOND_REDEMPTION_LIMIT);
		return FALSE;
	}

	return TRUE;
}

static int __cutvol_check_sge_instruction(const char *instruction_id)
{
	hash_t *h = hash_init(STR, NULL, NULL, NULL);
	int ret = get_sge_instrctions(g_core_data->db_conn, g_core_data->trade_date, h);
	if (ret) {
		printf("WARNING: [%s][%d] cut vol get sge instructions failed.\n", __FL__);
        SET_RESULT(SO_BAD);
        goto ERROR;
	}

	char *instr = NULL;
	ret = hash_find(h, (void *)instruction_id, (void **)&instr);
	if (ret) {
		printf("WARNING: [%s][%d] cut vol sge instructions already handled.\n", __FL__);
        SET_RESULT(INSTRUCTION_HANDLED);
        goto ERROR;
    }

    hash_destroy(h);
	return TRUE;

ERROR:
    hash_destroy(h);
    return FALSE;
}

static int __cutvol_check_trade_time()
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
	    printf("ERROR: [%s][%d] failed to find trade time !\n", __FL__);
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

	log_warn("it's not trade time now!");
	SET_RESULT(TRADE_TIME_ERR);
	array_destroy(a);
	return FALSE;

}

static int __cutvol_req_check(cut_vol_req_t *req)
{
	int ret;

	ret=__cutvol_check_trade_time(); // TODO:
	if (ret) {
		printf("WARNING: [%s][%d] cut vol check trade time failed.\n", __FL__);
		return FALSE;
	}
	
	ret = __cutvol_check_sge_instruction(req->instruction_id);
	if (ret) {
		printf("WARNING: [%s][%d] cut vol check sge instruction failed.\n", __FL__);
		return FALSE;
	}

	tbl_trade_list_t *tl = NULL;
	if (map_get(g_core_data->trade_list, req->instrument_id, (void **)&tl)) {
		SET_RESULT(ETF_CODE_NOT_FOUND);
		return FALSE;
	}

	ret = __cutvol_check_client(req);
	if (ret) {
		printf("WARNING: [%s][%d] cut vol check client failed.\n", __FL__);
		return FALSE;
	}

	ret = __cutvol_check_limit(req);
	if (ret) {
		printf("WARNING: [%s][%d] cut vol check client failed.\n", __FL__);
		return FALSE;
	}
	return TRUE;
}

int __cutvol_insert_info(cut_vol_req_t *req, cut_vol_rsp_t *rsp)
{
	tbl_trade_info_t trade_info;
	memset(&trade_info, 0, sizeof(tbl_trade_info_t));

	/* req */
	STRNCPY(trade_info.trade_date, g_core_data->trade_date);
	STRNCPY(trade_info.sge_instruc, req->instruction_id);
	trade_info.recv_type = RECV;
	trade_info.trans_no = req->msg_head.trans_no;
	trade_info.msg_type = CUT_VOL_REQ;
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
	trade_info.msg_type = CUT_VOL_RSP;

	insert_trade_info(g_core_data->db_conn, &trade_info);

	return TRUE;
}

int __cutvol_update_trade_vol(const char *etf_code, long long quantity)
{
	return update_trade_vol(g_core_data->db_conn, g_core_data->trade_date, etf_code, 0, quantity);
}
int __cutvol_update_client_quantity(const char *account_id,const char *pbu,long long quantity)
{
	tbl_client_t client;
	int ret = get_client(g_core_data->db_conn,account_id, &client);
	if (ret) {
		return FALSE;
	}
	return update_client_quantity(g_core_data->db_conn, account_id, pbu, quantity, client.status);
}

static int __cutvol_update_db(cut_vol_req_t *req, cut_vol_rsp_t *rsp)
{
	__cutvol_insert_info(req, rsp);

	if (strcmp(rsp->processing_result, CUTVOL_OK) == 0) {
		__cutvol_update_trade_vol(req->instrument_id, req->quantity);
		
		__cutvol_update_client_quantity(req->account_id,req->PBU,req->quantity); // TODO:
	}

	return TRUE;
}

int cut_vol_req_handler(shield_head_t *h)
{
	printf("TRACE: [%s][%d] cut vol handler called.\n", __FL__);

	CLEAR_RESULT();
	
	cut_vol_req_t *cut_vol_req = (cut_vol_req_t *)(h + 1);

	if (cut_vol_req->msg_head.trans_no <= g_core_data->recv_trans_no)
		return TRUE;

	g_core_data->recv_trans_no = cut_vol_req->msg_head.trans_no;

	int ret;
	ret = __cutvol_req_check(cut_vol_req);
	if (ret) {
		printf("WARNING: [%s][%d] cut vol check failed.\n", __FL__);
		goto AFTER;
	}

	SET_RESULT(REDEMPTION_SUCCESS);

AFTER:
	{
		CALLOC_MSG(cut_vol_rsp, h->fd, CUT_VOL_RSP);

		__package_cutvol_rsp_head(&cut_vol_rsp->msg_head);

		STRNCPY(cut_vol_rsp->processing_result, result_code);
		STRNCPY(cut_vol_rsp->description, result_desc);
		STRNCPY(cut_vol_rsp->org_instruction_id, cut_vol_req->instruction_id);
		STRNCPY(cut_vol_rsp->instrument_id, cut_vol_req->instrument_id);
		STRNCPY(cut_vol_rsp->PBU, cut_vol_req->PBU);
		STRNCPY(cut_vol_rsp->account_id, cut_vol_req->account_id);
		cut_vol_rsp->quantity = cut_vol_req->quantity;

		__cutvol_update_db(cut_vol_req, cut_vol_rsp);
		
		PUSH_MSG(cut_vol_rsp);
	}
	return 0;
}
	

