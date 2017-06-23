#include "db_handler.h"
#include "core.h"
#include "utils/utils.h"
#include "utils/log.h"
#include "include/trade_type.h"
#include "include/trade_msg.h"
#include "frame/frame.h"
#include "db/db.h"

#include <stdio.h>
#include <string.h>

core_data_t *g_core_data;
map_t       *fd_heart;

char result_code[8];
char result_desc[32];

static int __heart_beat_conf(map_t *cfg)
{
    g_core_data->sse_heart_beat = calloc(1, sizeof(heart_beat_conf_t));

    char *tmp = NULL;
    int  ret;
    ret = map_get(cfg, "sse_heart_beat_interval", (void **)&tmp);
    if (ret) {
        log_error("get sse_heart_beat_interval error."); 
        goto ERROR;
    }

    g_core_data->sse_heart_beat->interval = atoi(tmp);

    ret = map_get(cfg, "sse_heart_beat_lose", (void **)&tmp);
    if (ret) {
        log_error("get sse_heart_beat_interval error."); 
        goto ERROR;
    }

    g_core_data->sse_heart_beat->lose_interval = atoi(tmp);

    return 0;

ERROR:
    free(g_core_data->sse_heart_beat);
    g_core_data->sse_heart_beat = NULL;
    return -1;
}

int core_init(map_t *cfg)
{
    log_debug("------core init begin------");

	g_core_data = calloc(1, sizeof(core_data_t));
	if (g_core_data == NULL) {
		log_error("calloc g_core_data failed.");
		return -1;
	}

	if (db_conn_init(&(g_core_data->db_conn)) != 0) {
		log_error("init db error.");
		return -1;
	}

	if (get_trade_date(g_core_data->db_conn, g_core_data->trade_date) != 0) {
		log_error("get trade date error.");	
		return -1;
	}

    g_core_data->trade_list = map_init(STR, POINTER);
    if (get_trade_list(g_core_data->db_conn, g_core_data->trade_list, g_core_data->trade_date)) {
        log_error("get trade list error."); 
        return -1;
    }

    if (get_max_recv_no(g_core_data->db_conn, g_core_data->trade_date, &g_core_data->recv_trans_no)) {
		log_error("get max recv no error.");	
		return -1;
    }

    g_core_data->login_list = map_init(INT, INT);
    if (__heart_beat_conf(cfg)) {
        log_error("set heart beat conf error."); 
        goto ERROR;
    }

    if ((fd_heart = map_init(INT, POINTER)) == NULL) {
        log_error("fd heart map init error"); 
        goto ERROR;
    }

    log_debug("------core init end------");

    return 0;

ERROR:
    db_close(g_core_data->db_conn);

    if (g_core_data->trade_list != NULL)
        map_destroy(g_core_data->trade_list);

    if (g_core_data->login_list != NULL)
        map_destroy(g_core_data->login_list);

    free(g_core_data);
	return -1;
}

typedef int (*biz_handler)(shield_head_t *h);
static biz_handler __find_handler(int type)
{   
	int i;
	for (i = 0; g_hp[i].type != 0 && g_hp[i].handler != NULL; ++i)
		if (g_hp[i].type == type)
			return g_hp[i].handler;
    
    return NULL;
}

static int __exe(shield_head_t *h)
{
    int (*biz_handler)(shield_head_t *h);

    biz_handler = __find_handler(h->trade_type);

    if (biz_handler != NULL) {
        return biz_handler(h);
    } else {
        log_error("handler for trade_type [%lld] not found.", h->trade_type);
        return FALSE;
    }
    return TRUE;
}

static int __send_del_fd(int fd)
{
    shield_head_t *h = calloc(1, sizeof(shield_head_t));
    h->magic_num = MAGIC_NUM;
    h->fd = fd;
    h->len = 0;
    h->trade_type = CMD_DEL_FD;
    g_svr->core->push_to_middle(h);
    return 0;
}

static int __send_heart_beat(int fd)
{
    return send_ping(fd);
}

static int __update_fd_login(int fd)
{
    void *p = NULL;
    map_remove(g_core_data->login_list, (void *)&fd, (void **)&p);
    if (p)
        free(p);
    return 0;
}

static int __del_fd_handle(int fd)
{
    void *p = NULL;
    map_remove(fd_heart, (void *)&fd, (void **)&p);
    if (p)
        free(p);

    __update_fd_login(fd);

    return 0;
}

static int __lock_msg_handle()
{
    struct timeval curr;
    gettimeofday(&curr, NULL);

    size_t size = 0;
    void **keys = NULL;
    keys = map_keys(fd_heart, &size);
    size_t i;
    fd_heart_t *t;
    for (i = 0; i < size; ++i) {
        if (map_get(fd_heart, keys[i], (void **)&t) == 0) {
            if (t->had_sent && curr.tv_sec - t->last_beat.tv_sec >= g_core_data->sse_heart_beat->lose_interval) {
                __del_fd_handle(*(int *)keys[i]);
                __send_del_fd(*(int *)keys[i]);
            } else if (t->had_sent == 0 && curr.tv_sec - t->last_beat.tv_sec >= g_core_data->sse_heart_beat->interval)
                __send_heart_beat(*(int *)keys[i]);
        }
    }
    return 0;
}

static int __add_fd_handle(int fd)
{
    fd_heart_t *t = calloc(1, sizeof(fd_heart_t));
    gettimeofday(&t->last_beat, NULL);

    map_put(fd_heart, (void *)&fd, (void *)t);
    return 0;
}

static int __update_heart_beat(int fd)
{
    fd_heart_t *t = calloc(1, sizeof(fd_heart_t));
    gettimeofday(&t->last_beat, NULL);

    void *p = NULL;
    map_replace(fd_heart, (void *)&fd, (void *)t, &p);
    if (p)
        free(p);
    return 0;
}

int core_dispatch(shield_head_t *head)
{
    if (head->trade_type == CMD_CLOCK_MSG)
        return __lock_msg_handle();

    if (head->trade_type == CMD_ADD_FD)
        return __add_fd_handle(head->fd);

    if (head->trade_type == CMD_DEL_FD)
        return __del_fd_handle(head->fd);

    // check login

    msg_head_t *msg_h = (msg_head_t *)(head + 1); 
    if (head->trade_type == CMD_ADD_VOL_REQ 
        || head->trade_type == CMD_CUT_VOL_REQ
        || head->trade_type == CMD_TRADE_QRY_REQ) {
        if (msg_h->trans_no <= g_core_data->recv_trans_no)
            return TRUE;
    }
    g_core_data->recv_trans_no = msg_h->trans_no;

    __update_heart_beat(head->fd);

	return __exe(head);
}
