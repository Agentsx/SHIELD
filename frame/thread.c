#include "thread.h"
#include "frame.h"
#include "net.h"
#include "handler.h"
#include "utils/utils.h"
#include "utils/log.h"
#include "include/trade_type.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <unistd.h>

#define MAXFDS         1024
#define LEN_OF_MSGLEN  6 
#define SLEEPTIME      2000

thread_pool_t *tp = NULL;

static pthread_key_t __thread_key;

log_category_t *__get_log_category()
{
    return (log_category_t *)pthread_getspecific(__thread_key);
}

void thread_begin(const char *name)
{
    log_category_t *c = log_get_category(name);    
    pthread_setspecific(__thread_key, (void *)c);
    log_set_callback(__get_log_category);
    log_info("Thread begin.");
}

static int __fd_add(int fd)
{
    // PUSH TO R IN QUEUE
    void *item = calloc(1, sizeof(unsigned int) + sizeof(int));
    *(unsigned int *)item = READ_ADD_FD;
    *(int *)((unsigned int *)item + 1) = fd;
    return queue_push(tp->read_in, item);
}

static int __push_del_fd(int fd, queue *q)
{
    void *item = calloc(1, sizeof(unsigned int) + sizeof(int));
    *(unsigned int *)item = READ_DEL_FD; 
    *(int *)((unsigned int *)item + 1) = fd;
    return queue_push(q, item);
}

int push_to(void *msg, queue *q)
{
    return queue_push(q, msg);
}

int __socket_event_handler(int fd, struct epoll_event *event)
{
    if (event->events & EPOLLIN) {
        int connfd = accept_fd(fd);
        if (connfd > 0) {
            __fd_add(connfd);
			log_info("connect new fd [%d].", connfd);
		}

        shield_head_t *h = calloc(1, sizeof(shield_head_t));
        h->magic_num = MAGIC_NUM;
        h->trade_type = CMD_ADD_FD;
        h->fd = connfd;
        push_to(h, tp->middle_in);
    }
    return 0;
}

int __send_lock_msg()
{
    shield_head_t *head = calloc(1, sizeof(shield_head_t));
    head->magic_num = MAGIC_NUM;
    head->trade_type = CMD_CLOCK_MSG;
    g_svr->core->handler(head);
	return 0;
}

static void *__manage_routine(void *ctx)
{
    thread_begin("manage");

    int epfd = epoll_create(MAXFDS);
    struct epoll_event *events = calloc(MAXFDS, sizeof(struct epoll_event));
    struct epoll_event event;
    memset(&event, 0x00, sizeof(event));
    event.events |= EPOLLIN;
    event.events |= EPOLLERR | EPOLLHUP;
    event.data.fd = g_svr->listenfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, g_svr->listenfd, &event);
    while (1) {
        int num = epoll_wait(epfd, events, MAXFDS, 0);
        int i;
        for (i = 0; i < num; ++i) {
            int fd = events[i].data.fd;
            if (fd == g_svr->listenfd)
                __socket_event_handler(fd, &events[i]);
            else
                log_fatal("epoll event. fd[%d].", fd);
			log_debug("some one connected.");
        }

		int counter = 5;
		shield_head_t *h = NULL;
        while (counter--) {
			h = NULL;
            if (queue_pop(tp->read_out, (void **)&h))
				continue;

			if (h->magic_num == MAGIC_NUM) {
				push_to(h, tp->middle_in);
			} else {
				unsigned int st = *(unsigned int*)h;
				if (st == READ_DEL_FD) {
					int fd = *(int *)((unsigned int *)h + 1);
					close_fd(fd);
				} else {
					log_fatal("unkown st [%u].\n", st);
				}
				free(h);
			}
        }

		counter = 5;
		while (counter--) {
			h = NULL;
            if (queue_pop(tp->middle_out, (void **)&h))
				continue;

			if (h->magic_num == MAGIC_NUM) {
                if (h->trade_type > MAX_BIZ_CMD) {
                    if (h->trade_type == CMD_DEL_FD) {
                        log_notice("close fd[%d]", h->fd);
                        close(h->fd);
                        __push_del_fd(h->fd, tp->read_in);
                    }
                } else {
				    push_to(h, tp->write_in);
                }
			} else {
				log_fatal("read from middle out error.");	
				free(h);
			}
		}
		usleep(SLEEPTIME);
    }

    epoll_ctl(epfd, EPOLL_CTL_DEL, g_svr->listenfd, NULL);
    free(events);
    
    return NULL;
}

static void *__read_routine(void *ctx)
{
    thread_begin("read");

    int epfd = epoll_create(MAXFDS);
    struct epoll_event *events = calloc(MAXFDS, sizeof(struct epoll_event));

	int ret;
    while (1) {
        void *p;
        ret = queue_pop(tp->read_in, &p);
        if (ret == 0) {
            unsigned int st;
            int fd;
            st = *(unsigned int *)p;
            fd = *(int *)((unsigned int *)p + 1);
            if (st == READ_ADD_FD) {
                struct epoll_event event;
                memset(&event, 0x00, sizeof(event));
                event.events |= EPOLLIN;
                event.events |= EPOLLERR | EPOLLHUP;
                event.data.fd = fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
				log_info("read thread add new fd [%d].", fd);
            } else if (st == READ_DEL_FD) {
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                __push_del_fd(fd, tp->read_out);
            }
        }
        int num = epoll_wait(epfd, events, MAXFDS, 0);
        int i;
        for (i = 0; i < num; ++i) {
            int fd = events[i].data.fd;
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
				log_info("fd [%d] error.", fd);
                __push_del_fd(fd, tp->read_out);
            }
            if (events[i].events & EPOLLIN) {
				int fd = events[i].data.fd;
				size_t len = 0;
				log_info("read fd[%d] begin ......", fd);
				void *msg = tp->sse_protocol->pro_read(fd, &len);
				log_info("read fd[%d] end ......", fd);
                if (msg != NULL) { // good msg, put it
					log_info("read msg[%s] from fd length[%ld].", (char *)msg, len);
					shield_head_t *h = calloc(1, sizeof(shield_head_t) + len);
					h->magic_num = MAGIC_NUM;
					h->len = len;
					h->fd = fd;
                    memcpy(h + 1, msg, len);
                    push_to(h, tp->read_out);
               		free(msg);
                } else {
					// bad fd, delete it
					log_error("read msg from fd error.");
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                	__push_del_fd(fd, tp->read_out);
				}
            }
        }
    }

    return NULL;
}

static void *__write_routine(void *ctx)
{
    thread_begin("write");

   	shield_head_t *h;
    while (1) {
		h = NULL;
        if (queue_pop(tp->write_in, (void **)&h))
			continue;

        if (h->magic_num == MAGIC_NUM) {
        	log_info("will write to fd[%d], message[%s], length[%ld].", h->fd, (char *)(h + 1), h->len);
        	int ret;
        	ret = tp->sse_protocol->pro_write(h->fd, h + 1, h->len);
        	log_info("length[%d] wroten.\n", ret);
			free(h);
		} else {
        	log_error("read from write in error.");
		}
        log_info("head [%p].", h);
        usleep(SLEEPTIME);
    }
    
    return NULL;
}

static void *__core_routine(void *ctx)
{
    thread_begin("core");

	struct timeval tv_last = { 0 };
	struct timeval tv_curr = { 0 };

	gettimeofday(&tv_last, NULL);

    int ret;
	ret = g_svr->core->init(g_svr->cfg);
	if (ret) {
		log_error("core init error.");	
		g_svr->running = 0;
        exit(1);
	}
    shield_head_t *h;
    while (g_svr->running) {
        usleep(SLEEPTIME);

	    gettimeofday(&tv_curr, NULL);
        if (tv_curr.tv_sec - tv_last.tv_sec >= 1) {
            tv_last.tv_sec = tv_curr.tv_sec;
            tv_last.tv_usec = tv_curr.tv_usec;
			__send_lock_msg();
		}

        h = NULL;
        if (queue_pop(tp->core_in, (void **)&h))
			continue;

		log_info("core read something from core in.");
        if (h->magic_num == MAGIC_NUM) {
			ret = g_svr->core->handler(h);
			if (ret)
				log_error("core handle msg error [%d].", ret);
			else
				log_info("core handle msg OK.");
		} else {
			log_info("read from core in error.");
		}
    
        free(h);
    }

    return NULL;
}

/*
static void *__persistent_routine(void *ctx)
{
    thread_begin("persistent");
	shield_head_t *head;
	int ret;
    while (1) {
		head = NULL;
        if (queue_pop(tp->persistent_in, (void **)&head))
			continue;

		log_info("persistent read someting from core.");
        if (head->magic_num == MAGIC_NUM) {
			ret = g_svr->persistent->handler(head);

		} else {
			log_fatal("persistent handle msg error [%d].", ret);
		}
		free(head);

		usleep(SLEEPTIME);
    }
    
    return NULL;
}
*/

static void *__middle_routine(void *ctx)
{
    thread_begin("middle");

    int ret;
	ret = g_svr->middle->init(g_svr->cfg);
	if (ret) {
		log_error("core init error.");	
		g_svr->running = 0;
        exit(1);
	}
    shield_head_t *head = NULL;
    while (1) {
        int ret;
        int counter = 5;
        while (counter--) {
			head = NULL;
            if (queue_pop(tp->middle_in, (void **)&head))
				continue;

            if (head->magic_num == MAGIC_NUM) {
            	ret = g_svr->middle->handle_in(head);
				if (ret)
					log_error("middle handle in message error [%d].", ret);
			} else {
					log_error("read from middle in error.");
			}

            free(head);
        }

        counter = 5;
        while (counter--) {
			head = NULL;
            if (queue_pop(tp->core_out, (void **)&head))
				continue;

            if (head->magic_num == MAGIC_NUM) {
            	ret = g_svr->middle->handle_out(head);
				if (ret)
					log_error("middle handle out message error [%d].", ret);
			} else {
				log_error("read from core out message error [%d].", ret);
			}

            free(head);
        }

        usleep(SLEEPTIME);
        
    }
    return NULL;
}

static void __run()
{
    pthread_create(&tp->manage_tid, NULL, __manage_routine, NULL);
    pthread_create(&tp->read_tid, NULL, __read_routine, NULL);
    pthread_create(&tp->write_tid, NULL, __write_routine, NULL);
    pthread_create(&tp->core_tid, NULL, __core_routine, NULL);
    /* pthread_create(&tp->persistent_tid, NULL, __persistent_routine, NULL); */
    pthread_create(&tp->middle_tid, NULL, __middle_routine, NULL);
}

static void __join()
{
    pthread_join(tp->manage_tid, NULL);
    pthread_join(tp->read_tid, NULL);
    pthread_join(tp->write_tid, NULL);
    pthread_join(tp->core_tid, NULL);
    /* pthread_join(tp->persistent_tid, NULL); */
    pthread_join(tp->middle_tid, NULL);
}

void destroy_thread_pool(thread_pool_t *tpl)
{
    if (tpl) {
       if (tpl->read_in) queue_destroy(tpl->read_in);
       if (tpl->read_out) queue_destroy(tpl->read_out);
       if (tpl->write_in) queue_destroy(tpl->write_in);
       if (tpl->middle_in) queue_destroy(tpl->middle_in);
       if (tpl->middle_out) queue_destroy(tpl->middle_out);
       if (tpl->core_in) queue_destroy(tpl->core_in);
       if (tpl->core_out) queue_destroy(tpl->core_out);
       if (tpl->persistent_in) queue_destroy(tpl->persistent_in);
        free(tpl);
    }
}

thread_pool_t *thread_pool_init()
{
    tp = calloc(1, sizeof(thread_pool_t));

    tp->run = __run;
    tp->join = __join;

    if ((tp->read_in = queue_init()) == NULL) {
        log_fatal("queue init error.");
        goto ERROR;
    }
    if ((tp->read_out = queue_init()) == NULL) {
        log_fatal("queue init error.");
        goto ERROR;
    }
    if ((tp->write_in = queue_init()) == NULL) {
        log_fatal("queue init error.");
        goto ERROR;
    }
    if ((tp->middle_in = queue_init()) == NULL) {
        log_fatal("queue init error.");
        goto ERROR;
    }
    if ((tp->middle_out = queue_init()) == NULL) {
        log_fatal("queue init error.");
        goto ERROR;
    }
    if ((tp->core_in = queue_init()) == NULL) {
        log_fatal("queue init error.");
        goto ERROR;
    }
    if ((tp->core_out = queue_init()) == NULL) {
        log_fatal("queue init error.");
        goto ERROR;
    }

    /*
    if ((tp->persistent_in = queue_init()) == NULL) {
        log_fatal("queue init error.");
        return NULL;
    }
    */
    
    pthread_key_create(&__thread_key, NULL);

    thread_begin("main");

    return tp;
ERROR:
    destroy_thread_pool(tp);
    return NULL;
}
