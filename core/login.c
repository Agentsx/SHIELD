#include "core.h"
#include "middle/middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "err.h"
#include "include/tbl.h"
#include "utils/array.h"
#include "db_handler.h"
#include <stdio.h>
#include <string.h>

static int __auth_check(const char *user_name, const char *password)
{
    tbl_user_t user;

    int ret;
    ret = get_user(g_core_data->db_conn, user_name, &user);
    if (ret) {
	    printf("ERROR: [%s][%d] get user [%s] error.\n", __FL__, user_name);
        SET_RESULT(USER_NOT_FOUND);
        return -1;
    }

    if (strcmp(password, user.password)) {
	    printf("ERROR: [%s][%d] user_name[%s] wrong password[%s].\n", __FL__, user_name, password);
        SET_RESULT(WRONG_PASSWORD);
        return -1;
    }

    if (user.status != USER_OK) {
	    printf("ERROR: [%s][%d] user_name[%s] status not ok.\n", __FL__, user_name);
        SET_RESULT(USER_STATUS_ERR);
        return -1;
    }
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
        SET_RESULT(TRADE_DATE_ERR);
        return ret;
    }

    return 0;
}

static int __package_rsp_head(msg_head_t *h, const char *type, int msg_len, int body_len)
{
	h->msg_len = msg_len; 
	h->fix_length = NONFIX; 
	h->rec_length = body_len; 
	h->rec_no = 1;
	strncpy(h->msg_type, type, sizeof(h->msg_type));
	h->trans_no = 0;
	h->signature_flag = NONSIGNATURED;
	h->encrypted = NONENCRYTED;
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 
	return 0;
}

#define STRNCPY(a, b) strncpy(a, b, sizeof(a))

static int __send_rsp(shield_head_t *h, const tbl_trade_info_t *trade_info)
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

    switch (trade_info->msg_type) {
	case ADD_VOL_RSP: {
            CALLOC_MSG(add_vol_rsp, h->fd, trade_info->msg_type);
            __package_rsp_head(&add_vol_rsp->msg_head, A302, ADDVOL_RSP_LEN, ADDVOL_RSP_BODY_LEN);
            __package_trade_rsp_body(add_vol_rsp);

	        PUSH_MSG(add_vol_rsp);
        }
	case CUT_VOL_RSP: {
            CALLOC_MSG(cut_vol_rsp, h->fd, trade_info->msg_type);
            __package_rsp_head(&cut_vol_rsp->msg_head, A304, CUTVOL_RSP_LEN, CUTVOL_RSP_BODY_LEN);
            __package_trade_rsp_body(cut_vol_rsp);

	        PUSH_MSG(cut_vol_rsp);
        }
    }
    return 0;
}

static int __trans_no_handler(shield_head_t *h, long long begin_trans_no)
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

int login_req_handler(shield_head_t *h)
{
	printf("TRACE: [%s][%d] login req handler called.\n", __FL__);

    CLEAR_RESULT();
	
	login_req_t *login_req = (login_req_t *)(h + 1);

	int ret;

    ret = __login_check(login_req);
	if (ret)
        goto AFTER;

    __trans_no_handler(h, login_req->begin_trans_no);

    SET_RESULT(LOGIN_SUCCESS);

AFTER:
    {
	CALLOC_MSG(login_rsp, h->fd, LOGIN_RSP);

	__package_rsp_head(&login_rsp->msg_head, S202, LOGIN_RSP_LEN, LOGIN_RSP_BODY_LEN);

	login_rsp->result[0] = result_code[0];
	login_rsp->heart_bt_int = 0;
	strncpy(login_rsp->data_date, g_core_data->trade_date, sizeof(login_rsp->data_date));
	login_rsp->begin_trans_no = 0;
	strncpy(login_rsp->description, result_desc, sizeof(login_rsp->description));
	login_rsp->connection_type[0] = 'G';
	
	PUSH_MSG(login_rsp);
    }

	return 0;
}
