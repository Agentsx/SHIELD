#include "db_handler.h"
#include "core.h"

#include <stdio.h>
#include <string.h>

core_data_t *g_core_data;

int core_init()
{
	g_core_data = calloc(1, sizeof(core_data_t));
	if (g_core_data == NULL) {
		printf("ERROR: [%s][%d] calloc g_core_data failed.\n", __FL__);
		return -1;
	}

	if (db_conn_init(&(g_core_data->db_conn)) != 0) {
		printf("ERROR: [%s][%d] init db error.\n", __FL__);
		return -1;
	}

	if (get_trade_date(g_core_data->db_conn, g_core_data->trade_date) != 0) {
		printf("ERROR: [%s][%d] get trade date error.\n", __FL__);	
		return -1;
	}

	return 0;
}
