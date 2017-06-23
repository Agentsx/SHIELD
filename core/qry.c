#include "core.h"
#include "utils/log.h"
#include "db_handler.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include <string.h>

static int __package_head(msg_head_t *h)
{
	h->msg_len = LOGOUT_RSP_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = LOGOUT_RSP_BODY_LEN; 
	h->rec_no = 1; 
	strncpy(h->msg_type, A306, sizeof(h->msg_type));
	h->trans_no = 0; 
	h->signature_flag = NONSIGNATURED; 
	h->encrypted = NONENCRYTED; 
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 
	return 0;
}

static int __trade_qry_handle(trade_qry_req_t *req, trade_qry_rsp_t *rsp)
{
    int ret;
    tbl_trade_info_t trade_info;
    ret = get_trade_result(g_core_data->db_conn, g_core_data->trade_date, req->org_instruction_id, &trade_info);
    STRNCPY(rsp->org_instruction_id, req->org_instruction_id);
    if (ret == 0) {
        STRNCPY(rsp->processing_result, QRY_OK);
        STRNCPY(rsp->org_processing_result, trade_info.result_code);
        STRNCPY(rsp->org_description, trade_info.result_desc);
    } else {
        STRNCPY(rsp->processing_result, QRY_FAIL);
    }
    return 0;
}

int  trade_qry_req_handler(shield_head_t *h)
{
	log_notice("Trade qry handler called.");

    trade_qry_req_t *req = (trade_qry_req_t *)(h + 1);

	CALLOC_MSG(trade_qry_rsp, h->fd, TRADE_QRY_RSP);
	__package_head(&trade_qry_rsp->msg_head);

    __trade_qry_handle(req, trade_qry_rsp);

	PUSH_MSG(trade_qry_rsp);

	return 0;
}
