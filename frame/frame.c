#include "frame.h"
#include "handler.h"
#include "net.h"
#include "thread.h"
#include "protocol.h"
#include "utils/parsconf.h"
#include "utils/utils.h"
#include "utils/log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

svr_t  *g_svr;

static int __pars_svr_cfg(const char *cfg_file, map_t *m)
{
    return parse_config(cfg_file, m);
}

int server_init(char *cfg_file)
{
	g_svr = calloc(1, sizeof(svr_t));

    g_svr->cfg = map_init(STR, STR);
	if (__pars_svr_cfg(cfg_file, g_svr->cfg) < 0) {
		printf("ERROR: [%s][%d] read cfg file[%s] error.\n", __FL__, cfg_file);
		return -1;
	}

    char *s = NULL;
    if (map_get(g_svr->cfg, "log_conf", (void *)&s)) {
		printf("ERROR: [%s][%d] get listen port error.\n", __FL__);
        return -1; 
    }

    if (log_init(s)) {
		printf("ERROR: [%s][%d] init log[%s] error.\n", __FL__, s);
        return -1; 
    }

    if (map_get(g_svr->cfg, "listen_port", (void *)&s)) {
		printf("ERROR: [%s][%d] get listen port error.\n", __FL__);
		return -1;
    }
    int listenport = atoi(s);
    if (listenport < 1024 || listenport > 65535) {
		printf("ERROR: [%s][%d] listen port value[%d] error.\n", __FL__, listenport);
		return -1;
    }

	int listenfd;
	listenfd = init_net(listenport);
	if (listenfd < 0) {
		printf("ERROR: [%s][%d] initialize net error.\n", __FL__);
		return -1;
	}
    printf("TRACE: [%s][%d] net init end.\n", __FL__);

	g_svr->listenfd = listenfd;

	thread_pool_t *tp = thread_pool_init();
	if (tp == NULL) {
		printf("ERROR: [%s][%d] initialize threads pool error.\n", __FL__);
		return -1;
	}
    printf("TRACE: [%s][%d] poll init end.\n", __FL__);

	tp->sse_protocol = init_protocol();
	if (tp->sse_protocol == NULL) {
		printf("ERROR: [%s][%d] initialize protocol error.\n", __FL__);
		return -1;
	}

	g_svr->tp = tp;

	g_svr->middle = calloc(1, sizeof(middle_handler_t));
	g_svr->core = calloc(1, sizeof(core_handler_t));
	g_svr->persistent = calloc(1, sizeof(persistent_handler_t));

	g_svr->set_middle = init_middle_handler;
	g_svr->set_core = init_core_handler;
	g_svr->set_persistent = init_persistent_handler;

	g_svr->running = 1;

    printf("TRACE: [%s][%d] server init end.\n", __FL__);
	return 0;
}

int server_set_core_init(int (*init)(void *p))
{
	g_svr->core->init = init;
	return 0;
}

int server_start()
{
	g_svr->tp->run();
	return 0;
}

void server_join()
{
	g_svr->tp->join();
}

void server_free()
{
	if (g_svr != NULL) {
		if (g_svr->tp != NULL)
			free(g_svr->tp);
        map_destroy(g_svr->cfg);
		free(g_svr);
	}
}
