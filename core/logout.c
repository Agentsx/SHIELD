#include "core.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "utils/log.h"
#include <string.h>

static int __package_head(msg_head_t *h)
{
	h->msg_len = LOGOUT_RSP_BODY_LEN + MSG_HEAD_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = LOGOUT_RSP_BODY_LEN; 
	h->rec_no = 1; 
	strncpy(h->msg_type, MT_LOGOUT_RSP, sizeof(h->msg_type));
	h->trans_no = 0; 
	h->signature_flag = NONSIGNATURED; 
	h->encrypted = NONENCRYTED; 
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 
	return 0;
}

int logout_req_handler(shield_head_t *h)
{
	log_notice("==logout req handler begin==");
	
	CALLOC_MSG(logout_rsp, h->fd, CMD_LOGOUT_RSP, h->log_id);

	__package_head(&logout_rsp->msg_head);

    int login = 0;
    void *p = NULL;
    map_replace(g_core_data->login_list, (void *)&(h->fd), (void *)&login, (void **)&p);
    if (p)
        free(p);
	
	PUSH_MSG(logout_rsp);
	log_notice("==logout req handler end==");

	return 0;
}
