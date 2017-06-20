#ifndef __CORE_H__
#define __CORE_H__

#include "db/sqlite3/sqlite3.h"
#include "frame/frame.h"
#include "include/tbl.h"

#define USER_OK      1
#define USER_NOT_OK  0

#define CALLOC_MSG(s, ifd, type) \
	shield_head_t *__##s = calloc(1, sizeof(shield_head_t) + sizeof(s##_t)); \
	__##s->magic_num = MAGIC_NUM; \
	__##s->fd = ifd; \
	__##s->trade_type = type; \
	s##_t *s = (s##_t *)(__##s + 1);

#define PUSH_MSG(s) \
	g_svr->core->push_to_middle(__##s);


extern handler_map_t g_hp[];

typedef struct core_data_s {
	char       trade_date[16];
	long long  recv_trans_no;
	long long  send_trans_no;
	sqlite3    *db_conn;
    map_t      *trade_list;
} core_data_t;

extern core_data_t *g_core_data;

int  core_init();
int  login_req_handler(shield_head_t *h);
int  biz_over_req_handler(shield_head_t *h);
int  logout_req_handler(shield_head_t *h);
int  ping_req_handler(shield_head_t *h);
int  ping_rsp_handler(shield_head_t *h);
int  add_vol_req_handler(shield_head_t *h);
int  cut_vol_req_handler(shield_head_t *h);
int  trade_qry_req_handler(shield_head_t *h);

#endif
