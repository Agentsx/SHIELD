#include "core.h"
#include "middle/middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "err.h"
#include "tbl.h"
#include "utils/array.h"
#include "db_handler.h"
#include <stdio.h>
#include <string.h>

#define LOGIN_OK  '0'
#define LOGIN_ERR '1'

#define CLEAR_RESULT() {result_code = '0'; memset(result_desc, 0, sizeof(result_desc));}

#define CPY_RESULT_DESC(err) strncpy(result_desc, err, sizeof(result_desc))

char result_code;
char result_desc[32];

static int __auth_check(const char *user_name, const char *password)
{
    tbl_user_t user;

    int ret;
    ret = get_user(g_core_data->db_conn, user_name, &user);
    if (ret) {
	    printf("ERROR: [%s][%d] get user [%s] error.\n", __FL__, user_name);
        CPY_RESULT_DESC(USER_NOT_FOUND);
        return -1;
    }

    if (strcmp(password, user.password)) {
	    printf("ERROR: [%s][%d] user_name[%s] wrong password[%s].\n", __FL__, user_name, password);
        CPY_RESULT_DESC(WRONG_PASSWORD);
        return -1;
    }

    if (user.status != USER_OK) {
	    printf("ERROR: [%s][%d] user_name[%s] status not ok.\n", __FL__, user_name);
        CPY_RESULT_DESC(USER_STATUS_ERR);
        return -1;
    
    }
	return 0;
}

static int __package_login_rsp_head(msg_head_t *h)
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

static int __trade_date_check(const char *date)
{
    return strcmp(date, g_core_data->trade_date);
}

static int __login_check(login_req_t *login_req)
{
    int ret;
	ret = __auth_check(login_req->user_name, login_req->password);
    if (ret)
        return ret;

    ret = __trade_date_check(login_req->data_date);
    if (ret) {
        CPY_RESULT_DESC(TRADE_DATE_ERR);
        return ret;
    }

    return 0;
}

static int __package_trade_rsp_head(msg_head_t *h)
{
	h->msg_len = TRADE_RSP_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = TRADE_RSP_BODY_LEN; 
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

static int __send_rsp(shield_head *h, tbl_trade_info_t *trade_info)
{
    switch (trade_info->msg_type) {
	case ADD_VOL_RSP: {
            CALLOC_MSG(add_vol_rsp, h->fd, trade_info->msg_type);
        }
	case CUT_VOL_RSP: {
            CALLOC_MSG(cut_vol_rsp, h->fd, trade_info->msg_type);
        }
    }
    return 0;
}

static int __trans_no_handler(shield_head *h, long long begin_trans_no)
{
    if (begin_trans_no > 0 && begin_trans_no < g_core_data->recv_trans_no) {
        int ret;

        array_t *a = array_init(NULL);
        ret = get_send_trade_info_trans_no_greater_than(g_core_data->db_conn, begin_trans_no, a);
        if (ret) {
            printf("ERROR: [%s][%d] no trade info found, begin_trans_no[%lld].\n", __FL__, begin_trans_no);
            return -1;
        }

        int i;
        tbl_trade_info_t *trade_info = NULL;
        for (i = 0; i < array_count(a); ++i) {
            trade_info = (tbl_trade_info_t *)array_get(a, i); 
            __send_rsp(h, trade_info);
        }
    
        array_destroy(a);
    }
    return 0;
}

int login_req_handler(shield_head *h)
{
	printf("TRACE: [%s][%d] login req handler called.\n", __FL__);

    CLEAR_RESULT();
	
	login_req_t *login_req = (login_req_t *)(h + 1);

	int ret;

    ret = __login_check(login_req);
	if (ret) {
        result_code = LOGIN_ERR;
        goto AFTER;
    }

    __trans_no_handler(h, login_req->begin_trans_no);

AFTER:
    {
	CALLOC_MSG(login_rsp, h->fd, LOGIN_RSP);

	__package_login_rsp_head(&login_rsp->msg_head);

	login_rsp->result[0] = result_code;
	login_rsp->heart_bt_int = 0;
	strncpy(login_rsp->data_date, g_core_data->trade_date, sizeof(login_rsp->data_date));
	login_rsp->begin_trans_no = 0;
	strncpy(login_rsp->description, result_desc, sizeof(login_rsp->description));
	login_rsp->connection_type[0] = 'G';
	
	PUSH_MSG(login_rsp);
    }

	return 0;
}
