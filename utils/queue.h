#ifndef _QUEUE_H_
#define _QUEUE_H_

#define QUEUE_SIZE (1024 * 1024)

typedef struct queue_t {
    volatile int head;
    volatile int tail;
    void * volatile q[QUEUE_SIZE];
} queue;

queue *queue_init();
int  queue_push(queue *q, void *item);
int  queue_pop(queue *q, void **item);
void queue_destroy(queue *q);

#endif
