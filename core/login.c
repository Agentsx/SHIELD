#include "core.h"
#include "middle/middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include <stdio.h>
#include <string.h>

static int __auth_check(const char *user_name, const char *password)
{
	printf("TRACE: [%s][%d] user_name[%s] password[%s].\n", __FL__, user_name, password);
	return 0;
}

static int __package_head(msg_head_t *h)
{
	h->msg_len = LOGOUT_RSP_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = LOGOUT_RSP_BODY_LEN; 
	h->rec_no = 1; 
	strncpy(h->msg_type, S202, sizeof(h->msg_type));
	h->trans_no = 0; 
	h->signature_flag = NONSIGNATURED; 
	h->encrypted = NONENCRYTED; 
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 
	return 0;
}

int login_req_handler(shield_head *h)
{
	printf("TRACE: [%s][%d] login req handler called.\n", __FL__);
	
	login_req_t *login_req = (login_req_t *)(h + 1);

	int ret;
	ret = __auth_check(login_req->user_name, login_req->password);
	if (ret);

    // TODO:

	CALLOC_MSG(login_rsp, h->fd, LOGIN_RSP);

	__package_head(&login_rsp->msg_head);
	
	printf("TRACE: [%s][%d] login rsp head package ok.\n", __FL__);
	login_rsp->result[0] = '0';
	login_rsp->heart_bt_int = 0;
	strncpy(login_rsp->data_date, g_core_data->trade_date, sizeof(login_rsp->data_date));
	login_rsp->begin_trans_no = 0;
	strncpy(login_rsp->description, "login success", sizeof(login_rsp->description));
	login_rsp->connection_type[0] = 'G';
	
	printf("TRACE: [%s][%d] login rsp body package ok.\n", __FL__);
	printf("TRACE: [%s][%d] push to middle[%p].\n", __FL__, g_svr->core->push_to_middle);
	PUSH_MSG(login_rsp);

	return 0;
}
