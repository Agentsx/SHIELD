#include "core.h"
#include "middle/middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include <stdio.h>
#include <string.h>

static int __package_head(msg_head_t *h)
{
	h->msg_len = LOGOUT_RSP_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = LOGOUT_RSP_BODY_LEN; 
	h->rec_no = 1; 
	strncpy(h->msg_type, A304, sizeof(h->msg_type));
	h->trans_no = 0; 
	h->signature_flag = NONSIGNATURED; 
	h->encrypted = NONENCRYTED; 
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 
	return 0;
}

int cut_vol_req_handler(shield_head *h)
{
	printf("TRACE: [%s][%d] cut vol handler called.\n", __FL__);
		
	CALLOC_MSG(cut_vol_rsp, h->fd, CUT_VOL_RSP);

	__package_head(&cut_vol_rsp->msg_head);
	
	printf("TRACE: [%s][%d] cut vol head package ok.\n", __FL__);

	printf("TRACE: [%s][%d]cut vol rsp body package ok.\n", __FL__);
	printf("TRACE: [%s][%d] push to middle[%p].\n", __FL__, g_svr->core->push_to_middle);
	PUSH_MSG(cut_vol_rsp);

	return 0;
}

