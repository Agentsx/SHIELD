#ifndef __CORE_H__
#define __CORE_H__

#include "include/sqlite3.h"
#include "frame/frame.h"
#include "include/tbl.h"
#include "utils/map.h"
#include "utils/hash.h"
#include "string.h"

#include <sys/time.h>

#define TRUE  0
#define FALSE 1

#define RECV   0
#define SEND   1

#define USER_OK    1
#define CLIENT_OK  1

#define TRADE_OK "00001"
#define QRY_OK   "00001"
#define QRY_FAIL "10000"

#define LOGIN_OK '0'

extern char result_code[8];
extern char result_desc[32];


#define STRNCPY(a, b) strncpy(a, b, sizeof(a))

#define CLEAR_RESULT() \
    memset(result_code, 0, sizeof(result_code)); \
    memset(result_desc, 0, sizeof(result_desc));

#define __EC(c, m) c
#define __EM(c, m) m

#define _EC(x) __EC x
#define _EM(x) __EM x

#define SET_RESULT(x) \
	strncpy(result_code, _EC(x), sizeof(result_code)); \
	strncpy(result_desc, _EM(x), sizeof(result_desc));

#define CALLOC_MSG(s, ifd, type, logid) \
	shield_head_t *__##s = calloc(1, sizeof(shield_head_t) + sizeof(s##_t)); \
	__##s->magic_num = MAGIC_NUM; \
	__##s->log_id = logid; \
	__##s->fd = ifd; \
	__##s->trade_type = type; \
	s##_t *s = (s##_t *)(__##s + 1);

#define PUSH_MSG(s) \
	if (should_push()) {\
		g_svr->core->push_to_middle(__##s); \
	} else { \
		log_notice("core handle end, but will not push to middle.");	\
		free(__##s); \
	}

#define PUSH_LOGIN_RSP(s) \
	g_svr->core->push_to_middle(__##s);

#define PUSH_PING_RSP(s) \
	g_svr->core->push_to_middle(__##s);


typedef struct handler_map_s {
	int  type;
	int  (*handler)(shield_head_t *h);
} handler_map_t;

extern handler_map_t g_hp[];
extern map_t *fd_heart;

typedef struct fd_heart_s {
    int                had_sent;
    struct timeval     last_beat;
} fd_heart_t;

typedef struct heart_beat_conf_s {
    int                interval;
    int                lose_interval;
} heart_beat_conf_t;

typedef struct core_data_s {
	char               trade_date[16];
	long long          recv_trans_no;
	long long          send_trans_no;
    int                biz_over_flag;
	sqlite3            *db_conn;
    map_t              *trade_list;
    map_t              *login_list;
    heart_beat_conf_t  *heart_beat;
    hash_t             *instructions;
} core_data_t;

extern core_data_t *g_core_data;

int  core_init();
int  core_dispatch(shield_head_t *h);
int  login_req_handler(shield_head_t *h);
int  biz_over_req_handler(shield_head_t *h);
int  logout_req_handler(shield_head_t *h);
int  ping_req_handler(shield_head_t *h);
int  ping_rsp_handler(shield_head_t *h);
int  add_vol_req_handler(shield_head_t *h);
int  cut_vol_req_handler(shield_head_t *h);
int  trade_qry_req_handler(shield_head_t *h);
int  send_ping(int fd);
int  should_push();

#endif
