#include "db_handler.h"
#include "core.h"
#include "utils/utils.h"
#include "utils/log.h"
#include "include/trade_type.h"
#include "include/trade_msg.h"
#include "frame/frame.h"

#include <stdio.h>
#include <string.h>

core_data_t *g_core_data;

char result_code[8];
char result_desc[32];

int core_init()
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

    log_debug("------core init end------");

	return 0;
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

int core_dispatch(shield_head_t *head)
{
    msg_head_t *msg_h = (msg_head_t *)(head + 1); 
    if (head->trade_type == ADD_VOL_REQ 
        || head->trade_type == CUT_VOL_REQ
        || head->trade_type == TRADE_QRY_REQ) {
        if (msg_h->trans_no <= g_core_data->recv_trans_no)
            return TRUE;
    }
    g_core_data->recv_trans_no = msg_h->trans_no;

	return __exe(head);
}
