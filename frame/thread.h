#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include "utils/queue.h"
#include "protocol.h"

#define READ_ADD_FD 1
#define READ_DEL_FD 2
#define GOOD_MSG 2

typedef struct thread_pool_s {
    pthread_t manage_tid;
    pthread_t read_tid;
    pthread_t write_tid;
    pthread_t middle_tid;
    pthread_t core_tid;
    pthread_t persistent_tid;

    queue *read_in;
    queue *read_out;
    queue *write_in;
    queue *middle_in;
    queue *middle_out;
    queue *core_in;
    queue *core_out;
    queue *persistent_in;

    void (*run)();
    void (*join)();

	protocol_t *sse_protocol;
	
} thread_pool_t;

extern thread_pool_t *tp;

thread_pool_t *thread_pool_init();
void destroy_thread_pool(thread_pool_t *tpl);

#endif
