#include "db/db.h"
#include "utils/hash.h"
#include "utils/array.h"
#include "db_handler.h"
#include "frame/frame.h"

#include <stdio.h>
#include <string.h>

int db_conn_init(sqlite3 **conn)
{
	return db_init(DB_NAME, conn);
}

int get_trade_date(sqlite3 *conn, char *date)
{
	char *sql = "select max(f_trade_date) from t_trade_date;";

	array_t *a = array_init((array_item_destroy)hash_destroy);

	int ret = 0;
	char *err_msg = NULL;
	ret = db_exec_dql(conn, sql, &err_msg, a);
	if (ret != 0) {
		printf("ERROR: [%s][%d] select trade date error.\n" , __FL__);	
		goto ERROR;
	}
	if (array_count(a) == 0)
		goto ERROR;

	hash_t *h = (hash_t *)array_get(a, 0);
	if (h == NULL)
		goto ERROR;

	char *trade_date = NULL;
	ret = hash_get(h, "f_trade_date", (void **)&trade_date);
	if (ret != 0 || trade_date == NULL)
		goto ERROR;

	strncpy(date, trade_date, 8);
	array_destroy(a);
	return 0;

ERROR:
	array_destroy(a);
	return -1;
}
