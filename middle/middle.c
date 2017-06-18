#include "frame/frame.h"
#include "middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

template_t head_template[] = {
	{6, 'L', 0},
	{1, 'L', 0},
	{6, 'L', 0},
	{6, 'L', 0},
	{4, 'S', 8},
	{16, 'L', 0},
	{1, 'L', 0},
	{1, 'L', 0},
	{1, 'L', 0},
	{10, 'S', 16},
	{128, 'S', 136},
	{-1, -1, -1}
};

template_t login_req_template[] = {
	{10, 'S', 16},
	{16, 'S', 24},
	{16, 'L', 0},
	{8, 'S', 16},
	{10, 'L', 16},
	{1, 'S', 8},
	{-1, -1, -1}
};

template_t login_rsp_template[] = {
	{1, 'S', 8},
	{16, 'L', 0},
	{8, 'S', 16},
	{16, 'L', 0},
	{21, 'S', 24},
	{1, 'S', 8},
	{-1, -1, -1}
};
template_t biz_over_req_template[] = {
	{3, 'S', 8},
	{16, 'L', 0},
	{1, 'S', 8},
	{-1, -1, -1}
};
template_t biz_over_rsp_template[] = {
	{3, 'S', 8},
	{16, 'L', 0},
	{1, 'S', 8},
	{-1, -1, -1}
};	
template_t logout_req_template[] = {
	{1, 'S', 8},
	{64, 'S', 64},
	{-1, -1, -1}
};
template_t logout_rsp_template[] = {
	{-1, -1, -1}
};

template_t ping_req_template[] = {
	{16, 'S', 16},
	{8, 'S', 8},
	{-1, -1, -1}
};
template_t ping_rsp_template[] = {
	{16, 'S', 16},
	{8, 'S', 8},
	{-1, -1, -1}
};
template_t add_vol_req_template[] = {
	{16, 'S', 16},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};
template_t add_vol_rsp_template[] = {
	{5, 'S', 8},
	{40, 'S', 40},
	{16, 'S', 16},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};
template_t cut_vol_req_template[] = {
	{16, 'S', 16},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};
template_t cut_vol_rsp_template[] = {
	{5, 'S', 8},
	{40, 'S', 40},
	{16, 'S', 16},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};
template_t trade_qry_req_template[] = {
	{16, 'S', 16},
	{-1, -1, -1}
};
template_t trade_qry_rsp_template[] = {
	{8, 'S', 16},
	{16, 'S', 16},
	{8, 'S', 16},
	{40, 'S', 40},
	{-1, -1, -1}
};
static int __resolve_msg(void *in, const char *msg, template_t *temp)
{
	char tmp[129];

	int mp = 0;
	int sp = 0;
	int i;
	for (i = 0; temp[i].len_in_msg != -1; ++i) {
		memset(tmp, 0, sizeof(tmp));
		STRTRIMNCPY(tmp, msg + mp, temp[i].len_in_msg);
		switch (temp[i].type) {
		case 'L':
			*(long long *)((char *)in + sp) = atol(tmp);
			sp += sizeof(long long);
			printf("TRACE: [%s][%d] tmp [%s].\n", __FL__, tmp);
			break;
		case 'S':
			strncpy((char *)in + sp, tmp, temp[i].len_in_struct);
			sp += temp[i].len_in_struct;
			printf("TRACE: [%s][%d] tmp [%s].\n", __FL__, tmp);
			break;
		}
		mp += temp[i].len_in_msg;
	}
	return 0;
}

static int __get_type(const char *msg_type)
{
	if (!strcmp(msg_type, S201))
		return LOGIN_REQ;
	else if (!strcmp(msg_type, S205))
		return BIZ_OVER_REQ;
	else if (!strcmp(msg_type, S209))
		return LOGOUT_REQ;
	else if (!strcmp(msg_type, S211))
		return PING_REQ;
	else if (!strcmp(msg_type, A301))
		return ADD_VOL_REQ;
	else if (!strcmp(msg_type, A303))
		return CUT_VOL_REQ;
	else if (!strcmp(msg_type, A305))
		return TRADE_QRY_REQ;

	return -1;
}

static msg_head_t * __resolve_head(const char *msg)
{
	msg_head_t *h = calloc(1, sizeof(msg_head_t));

	__resolve_msg(h, msg, head_template);

	return h;
}


static void *__resolve_body(long long type, const char *body, size_t *len)
{;
	void *new = NULL;
	switch (type) {
		case LOGIN_REQ:
			new = calloc(1, sizeof(login_req_t));
			*len = sizeof(login_req_t);
			__resolve_msg((char *)new + sizeof(msg_head_t), body, login_req_template);
			break;
		case BIZ_OVER_REQ:
			new = calloc(1, sizeof(biz_over_req_t));
			*len = sizeof(biz_over_req_t);
			__resolve_msg((char *)new + sizeof(msg_head_t), body, biz_over_req_template);
			break;
		case LOGOUT_REQ:
			new = calloc(1, sizeof(logout_req_t));
			*len = sizeof(logout_req_t);
			__resolve_msg((char *)new + sizeof(msg_head_t), body, logout_req_template);
			break;
		case PING_REQ:
			new = calloc(1, sizeof(ping_req_t));
			*len = sizeof(ping_req_t);
			__resolve_msg((char *)new + sizeof(msg_head_t), body, ping_req_template);
			break;
		case ADD_VOL_REQ:
			new = calloc(1, sizeof(add_vol_req_t));
			*len = sizeof(add_vol_req_t);
			__resolve_msg((char *)new + sizeof(msg_head_t), body, add_vol_req_template);
			break;
		case CUT_VOL_REQ:
			new = calloc(1, sizeof(cut_vol_req_t));
			*len = sizeof(cut_vol_req_t);
			__resolve_msg((char *)new + sizeof(msg_head_t), body, add_vol_req_template);
			break;
		case TRADE_QRY_REQ:
			new = calloc(1, sizeof(trade_qry_req_t));
			*len = sizeof(trade_qry_req_t);
			__resolve_msg((char *)new + sizeof(msg_head_t), body, trade_qry_req_template);
			break;
		default:
			printf("ERROR: [%s][%d] unkown message type [%lld].\n", __FL__, type);
			return NULL;
	}
	return new;
}

int resolve_msg(shield_head *head)
{
	char *msg = (char *)(head + 1);
	msg_head_t *h = __resolve_head(msg);
	if (h == NULL) {
		printf("ERROR: [%s][%d] resolve msg head [%s] error.\n", __FL__, msg);
		return -1;
	}

	long long trade_type;
	if ((trade_type = __get_type(h->msg_type)) == -1) {
		printf("ERROR: [%s][%d] message type error [%s].\n", __FL__, h->msg_type);
		free(h);	
		return -1;
	}

	size_t len;
	void *b = __resolve_body(trade_type, msg + MSG_HEAD_LEN, &len);
	if (b == NULL) {
		free(h);
		printf("ERROR: [%s][%d] resolve msg body [%s] error.\n", __FL__, msg + MSG_HEAD_LEN);
		return -1;
	}

	memcpy(b, h, sizeof(msg_head_t));

	shield_head *newh = calloc(1, sizeof(shield_head) + len);
	newh->fd = head->fd;
	newh->magic_num = head->magic_num;
	newh->len = len;
	newh->trade_type = trade_type;
	memcpy(newh + 1, b, len);

	free(h);
	free(b);

	MIDDLE_PUSH_IN(newh);
	printf("TRACE: [%s][%d] middle resolve msg ok.\n", __FL__);

	return 0;
}

static int __package_msg(void *h, char *msg, template_t *temp)
{
	char tmp[129];

	int mp = 0;
	int sp = 0;
	int i;
	char *p;
	size_t len;

	for (i = 0; temp[i].len_in_msg != -1; ++i) {
		memset(tmp, 0, sizeof(tmp));
		switch (temp[i].type) {
		case 'L':
			snprintf(tmp, sizeof(tmp), "%lld", *(long long *)((char *)h + sp));
			memcpy(msg + mp, tmp, strlen(tmp));
			sp += sizeof(long long);
			break;
		case 'S':
			p = (char *)h + sp;
			len = strlen(p) > temp[i].len_in_msg ? temp[i].len_in_msg : strlen(p); 
			memcpy(msg + mp, p, len);
			sp += temp[i].len_in_struct;
			break;
		}
		mp += temp[i].len_in_msg;
	}
	return 0;
}

char *__package_head(msg_head_t *head)
{
	char *h = calloc(1, MSG_HEAD_LEN + 1);
	
	memset(h, 0x20, MSG_HEAD_LEN);
	__package_msg(head, h, head_template);

	return h;
}

char *__package_body(long long type, msg_head_t *h, size_t *len)
{
	char *msg = NULL;

	switch (type) {
	case LOGIN_RSP:
		printf("TRACE: [%s][%d] package 'login rsp' body.\n", __FL__);
		msg = calloc(1, LOGIN_RSP_BODY_LEN + 1);
		*len = LOGIN_RSP_BODY_LEN;
		memset(msg, 0x20, LOGIN_RSP_BODY_LEN);
		__package_msg(h + 1, msg, login_rsp_template);
		break;
	case BIZ_OVER_RSP:
		printf("TRACE: [%s][%d] package 'biz over rsp' body.\n", __FL__);
		msg = calloc(1, BIZ_OVER_RSP_BODY_LEN + 1);
		*len = BIZ_OVER_RSP_BODY_LEN;
		memset(msg, 0x20, BIZ_OVER_RSP_BODY_LEN);
		__package_msg(h + 1, msg, biz_over_rsp_template);
		break;
	case LOGOUT_RSP:
		printf("TRACE: [%s][%d] package 'logout rsp' body.\n", __FL__);
		msg = calloc(1, LOGOUT_RSP_BODY_LEN + 1);
		*len = LOGOUT_RSP_BODY_LEN;
		memset(msg, 0x20, LOGOUT_RSP_BODY_LEN);
		__package_msg(h + 1, msg, logout_rsp_template);
		break;
	case PING_REQ:
		printf("TRACE: [%s][%d] package 'ping req' body.\n", __FL__);
		msg = calloc(1,PING_REQ_BODY_LEN + 1);
		*len = PING_REQ_BODY_LEN;
		memset(msg, 0x20, PING_REQ_BODY_LEN);
		__package_msg(h + 1, msg, ping_req_template);		
		break;
	case PING_RSP:
		printf("TRACE: [%s][%d] package 'ping rsp' body.\n", __FL__);
		msg = calloc(1, PING_RSP_BODY_LEN + 1);
		*len = PING_RSP_BODY_LEN;
		memset(msg, 0x20,PING_RSP_BODY_LEN);
		__package_msg(h + 1, msg, ping_rsp_template);		
		break;
	case ADD_VOL_RSP:
		printf("TRACE: [%s][%d] package 'add vol rsp' body.\n", __FL__);
		msg = calloc(1, ADD_VOL_RSP_BODY_LEN + 1);
		*len = ADD_VOL_RSP_BODY_LEN;
		memset(msg, 0x20, ADD_VOL_RSP_BODY_LEN);
		__package_msg(h + 1, msg, add_vol_rsp_template);		
		break;
	case CUT_VOL_RSP:
		printf("TRACE: [%s][%d] package 'cut vol rsp' body.\n", __FL__);
		msg = calloc(1, CUT_VOL_RSP_BODY_LEN + 1);
		*len = CUT_VOL_RSP_BODY_LEN;
		memset(msg, 0x20, CUT_VOL_RSP_BODY_LEN);
		__package_msg(h + 1, msg, cut_vol_rsp_template);		
		break;
	case TRADE_QRY_RSP:
		printf("TRACE: [%s][%d] package 'qry rsp' body.\n", __FL__);
		msg = calloc(1,TRADE_QRY_RSP_BODY_LEN + 1);
		*len = TRADE_QRY_RSP_BODY_LEN;
		memset(msg, 0x20, TRADE_QRY_RSP_BODY_LEN);
		__package_msg(h + 1, msg, trade_qry_rsp_template);		
		break;
	default:
		printf("ERROR: [%s][%d] package msg error trade type [%lld].\n", __FL__, type);
		msg = calloc(1, LOGIN_RSP_BODY_LEN + 1);
		*len = LOGIN_RSP_BODY_LEN;
		memset(msg, 0x20, LOGIN_RSP_BODY_LEN);
		__package_msg(h + 1, msg, login_rsp_template);	
		return NULL;
	}

	return msg;
}

int package_msg(shield_head *head)
{
	printf("TRACE: [%s][%d] middle package msg been called.\n", __FL__);

	char *h = __package_head((msg_head_t *)(head + 1));

	size_t len = 0;
	void *b = __package_body(head->trade_type, (msg_head_t *)(head + 1), &len);
	if (b == NULL) {
		free(h);
		printf("ERROR: [%s][%d] package msg body error.\n", __FL__);
		return -1;
	}

	shield_head *out = calloc(1, sizeof(shield_head) + MSG_HEAD_LEN + len);
	out->magic_num = head->magic_num;
	out->fd = head->fd;
	out->trade_type = head->trade_type;
	out->len = MSG_HEAD_LEN + len;
	memcpy(out + 1, h, MSG_HEAD_LEN);
	memcpy((char *)(out + 1) + MSG_HEAD_LEN, b, len);

	free(h);
	free(b);

	printf("TRACE: [%s][%d] middle package msg[%s] ok.\n", __FL__, (char *)(out + 1));
	printf("TRACE: [%s][%d] out[%p].\n", __FL__, out);

	MIDDLE_PUSH_OUT(out);

	return 0;
}
