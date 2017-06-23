#ifndef _FRAME_H_
#define _FRAME_H_

#include <stdlib.h>
#include "thread.h"
#include "utils/map.h"

#define __FL__ __FILE__, __LINE__

#define MAGIC_NUM 980234


typedef struct shield_head_s {
	unsigned int magic_num;
	size_t       len;
	int          fd;
	long long    trade_type;
} shield_head_t;

typedef struct core_s {
	int (*init)(map_t *cfg);
	int (*push_to_middle)(shield_head_t *h);
	int (*push_to_persistent)(shield_head_t *h);
	int (*handler)(shield_head_t *h);
} core_handler_t;

typedef struct middle_s {
    int  (*init)();
	int  (*handle_in)(shield_head_t *h);
	int  (*handle_out)(shield_head_t *h);
	int  (*push_in)(shield_head_t *h);
	int  (*push_out)(shield_head_t *h);
} middle_handler_t;

typedef struct persistent_s {
	int  (*handler)(shield_head_t *h);
} persistent_handler_t;

typedef struct svr_s {
	int                  listenfd;
	int                  running;
	map_t                *cfg;
	thread_pool_t        *tp;

	core_handler_t       *core;
	middle_handler_t     *middle;
	persistent_handler_t *persistent;

	int  (*set_core)(core_handler_t *core, int (*dispatch)(shield_head_t *head));
	int  (*set_middle)(middle_handler_t *middle,
                       int (*init)(),
                       int (*fin)(shield_head_t *),
                       int (*fout)(shield_head_t *));
	int  (*set_persistent)(persistent_handler_t *persistent,
	                       int (*handler)(shield_head_t *));
} svr_t;

extern svr_t *g_svr;

int  server_init(char *cfg_file);
int  server_start();
void server_join();
void server_free();

int server_set_core_init(int (*init)(map_t *p));

#endif
