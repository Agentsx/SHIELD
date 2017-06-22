#include "handler.h"
#include "frame.h"
#include "utils/queue.h"
#include <stdio.h>
#include "utils/log.h"

int middle_push_in(shield_head_t *h)
{
	return queue_push(g_svr->tp->core_in, h);
}

int middle_push_out(shield_head_t *h)
{
	return queue_push(g_svr->tp->middle_out, h);
}

int core_push_to_middle(shield_head_t *h)
{
	return queue_push(g_svr->tp->core_out, h);
}

int core_push_to_persistent(shield_head_t *h)
{
	return queue_push(g_svr->tp->persistent_in, h);
}

typedef int (*biz_handler)(shield_head_t *h);
biz_handler find_core_handler(int type)
{
	if (type <= 0 && type > MAX_TYPE)
		return NULL;

	return g_svr->core->handlers[type];
}

int core_handler_exe(shield_head_t *h)
{
	int (*biz_handler)(shield_head_t *h);

	biz_handler = find_core_handler(h->trade_type); 

	if (biz_handler != NULL) {
		return biz_handler(h);
	} else {
		log_error("handler for trade_type [%lld] not found.", h->trade_type);
		return -1;
	}
	return 0;	
}

int init_core_handler(core_handler_t *core, handler_map_t *m)
{
	int i;	
	for (i = 0; m[i].type != 0 && m[i].handler != NULL; ++i) {
		core->handlers[m[i].type] = m[i].handler;
	}

	core->handler = core_handler_exe;
	core->push_to_middle = core_push_to_middle;
	core->push_to_persistent = core_push_to_persistent;
	return 0;
}

int init_middle_handler(middle_handler_t *middle, int (*fin)(shield_head_t *), int (*fout)(shield_head_t *))
{
	middle->handle_in = fin;
	middle->handle_out = fout;
	middle->push_in = middle_push_in;
	middle->push_out = middle_push_out;
	return 0;
}


int init_persistent_handler(persistent_handler_t *persistent, int (*handler)(shield_head_t *))
{
	persistent->handler = handler;
	return 0;
}
