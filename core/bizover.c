#include "core.h"
#include "middle/middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include <stdio.h>
#include <string.h>


static int __bizover_package_head(msg_head_t *h)
{
	h->msg_len = BIZOVER_RSP_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = BIZOVER_RSP_BODY_LEN; 
	h->rec_no = 1; 
	strncpy(h->msg_type, S206, sizeof(h->msg_type));
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
	array_t *a = array_init(NULL);
	ret = get_trade_count(g_core_data->db_conn , g_core_data->trade_date, a);
	if (ret) {
	    log_error("ERROR: [%s][%d] failed to find trade count !\n", __FL__);
	    return -1;
	}

	STRNCPY(rsp->biz_code, req->biz_code);
	rsp->total_records=array_count(a);

	if(req->total_records<array_count(a)){
		STRNCPY(rsp->tran_status,"1");
	}
	if else(req->total_records==array_count(a)){
		STRNCPY(rsp->tran_status,"0");
	}
	else
		STRNCPY(rsp->tran_status, "2");
    return 0;
}


int  biz_over_req_handler(shield_head_t *h)
{
	log_notice("TRACE: [%s][%d] biz over req handler called.\n", __FL__);

	biz_over_req_t *req = (biz_over_req_t *)(h + 1);
   	
	CALLOC_MSG(biz_over_rsp, h->fd, BIZ_OVER_RSP);

	__bizover_package_head(&biz_over_rsp->msg_head);
	
	__biz_over_handle(req, biz_over_rsp);
	
	log_notice("TRACE: [%s][%d] push to middle[%p].\n", __FL__, g_svr->core->push_to_middle);

	PUSH_MSG(biz_over_rsp);

	return 0;
}
