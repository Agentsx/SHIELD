#include "db_handler.h"
#include "core.h"
#include "utils/utils.h"
#include "utils/log.h"

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
		log_error("init db error.", __FL__);
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
