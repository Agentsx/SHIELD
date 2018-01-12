#include "core.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "db_handler.h"
#include "utils/array.h"
#include "utils/log.h"
#include <stdio.h>
#include <string.h>


static int __bizover_package_head(msg_head_t *h)
{
	h->msg_len = BIZ_OVER_RSP_BODY_LEN + MSG_HEAD_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = BIZ_OVER_RSP_BODY_LEN; 
	h->rec_no = 1; 
	strncpy(h->msg_type, MT_BIZ_OVER_RSP, sizeof(h->msg_type));
	h->trans_no = 0; 
	h->signature_flag = NONSIGNATURED; 
	h->encrypted = NONENCRYTED; 
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 
	return 0;
}

static int __biz_over_handle(biz_over_req_t *req, biz_over_rsp_t *rsp)
{
    int ret;
	size_t count = 0;
	ret = get_trade_count(g_core_data->db_conn, g_core_data->trade_date, &count);
	if (ret) {
		log_error("failed to find trade count!");
		STRNCPY(rsp->tran_status, "2");		
	    return -1;
	}

	STRNCPY(rsp->biz_code, req->biz_code);
	rsp->total_records = count;

	if (req->total_records != count){
		STRNCPY(rsp->tran_status, "1");
	} else {
		STRNCPY(rsp->tran_status, "0");
	}

    return 0;
}


int  biz_over_req_handler(shield_head_t *h)
{
	log_notice("==biz over req handler begin==");

	biz_over_req_t *req = (biz_over_req_t *)(h + 1);
   	
	CALLOC_MSG(biz_over_rsp, h->fd, CMD_BIZ_OVER_RSP, h->log_id);

	__bizover_package_head(&biz_over_rsp->msg_head);
	
	__biz_over_handle(req, biz_over_rsp);
	
	log_notice("push to middle[%p].", g_svr->core->push_to_middle);

	PUSH_MSG(biz_over_rsp);
	log_notice("==biz over req handler end, rsp[%s]==", result_code);

	return 0;
}
