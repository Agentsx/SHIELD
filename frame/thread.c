#include "thread.h"
#include "frame.h"
#include "net.h"
#include "handler.h"
#include <stdio.h>
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

void thread_begin(const char *name)
{
    pthread_setspecific(__thread_key, )
    printf("TRACE: Thread [%s] begin.\n", name);
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
			printf("TRACE: [%s][%d] connect new fd [%d].\n", __FL__, connfd);
		}
    }
    return 0;
}

int __send_heart_beat()
{
	printf("TRACE: [%s][%d] send heart beat.\n", __FL__);
	
	return 0;
}

static void *__manage_routine(void *ctx)
{
    thread_begin("manage");

	struct timeval tv_last = { 0 };
	struct timeval tv_curr = { 0 };

	gettimeofday(&tv_last, NULL);

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
                printf("FATAL: [%s][%d] epoll event. fd[%d].\n", __FL__, fd);
			printf("DEBUG: [%s][%d] some one connected.\n", __FL__);
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
					printf("FATAL: [%s][%d] unkown st [%u].\n", __FL__, st);
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
				push_to(h, tp->write_in);
			} else {
				printf("FATAL: [%s][%d] read from middle out error.\n", __FL__);	
				free(h);
			}
			gettimeofday(&tv_last, NULL);
		}

		gettimeofday(&tv_curr, NULL);
		if (tv_curr.tv_sec - tv_last.tv_sec >= 40) {
			__send_heart_beat();
			gettimeofday(&tv_last, NULL);
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
				printf("TRACE: [%s][%d] read thread add new fd [%d].\n", __FL__, fd);
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
				printf("TRACE: [%s][%d] fd [%d] error.\n", __FL__, fd);
                __push_del_fd(fd, tp->read_out);
            }
            if (events[i].events & EPOLLIN) {
				int fd = events[i].data.fd;
				size_t len = 0;
				printf("TRACE: [%s][%d] read fd[%d] begin ......\n", __FL__, fd);
				void *msg = tp->sse_protocol->pro_read(fd, &len);
				printf("TRACE: [%s][%d] read fd[%d] end ......\n", __FL__, fd);
                if (msg != NULL) { // good msg, put it
					printf("TRACE: [%s][%d] read msg[%s] from fd length[%ld].\n", __FL__, (char *)msg, len);
					shield_head_t *h = calloc(1, sizeof(shield_head_t) + len);
					h->magic_num = MAGIC_NUM;
					h->len = len;
					h->fd = fd;
                    memcpy(h + 1, msg, len);
                    push_to(h, tp->read_out);
               		free(msg);
                } else {
					// bad fd, delete it
					printf("ERROR: [%s][%d] read msg from fd error.\n", __FL__);
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
        	printf("TRACE: [%s][%d] will write to fd[%d], message[%s], length[%ld].\n", __FL__, h->fd, (char *)(h + 1), h->len);
        	int ret;
        	ret = tp->sse_protocol->pro_write(h->fd, h + 1, h->len);
        	printf("TRACE: [%s][%d] length[%d] wroten.\n", __FL__, ret);
			free(h);
		} else {
        	printf("ERROR: [%s][%d] read from write in error.\n", __FL__);
		}
        printf("TRACE: [%s][%d] head [%p].\n", __FL__, h);
        usleep(SLEEPTIME);
    }
    
    return NULL;
}

static void *__core_routine(void *ctx)
{
    thread_begin("core");

    int ret;
	ret = g_svr->core->init(NULL);
	if (ret) {
		printf("ERROR: [%s][%d] core init error [%d].\n", __FL__, ret);	
		g_svr->running = 0;
	}
    shield_head_t *h;
    while (g_svr->running) {
        usleep(SLEEPTIME);
        h = NULL;
        if (queue_pop(tp->core_in, (void **)&h))
			continue;

		printf("TRACE: [%s][%d] core read something from core in.\n", __FL__);
        if (h->magic_num == MAGIC_NUM) {
			ret = g_svr->core->handler(h);
			if (ret)
				printf("ERROR: [%s][%d] core handle msg error [%d].\n", __FL__, ret);
			else
				printf("TRACE: [%s][%d] core handle msg OK.\n", __FL__);
		} else {
			printf("TRACE: [%s][%d] read from core in error.\n", __FL__);
		}
    
        free(h);
    }

    return NULL;
}

static void *__persistent_routine(void *ctx)
{
    thread_begin("persistent");
	shield_head_t *head;
	int ret;
    while (1) {
		head = NULL;
        if (queue_pop(tp->persistent_in, (void **)&head))
			continue;

		printf("TRACE: [%s][%d] persistent read someting from core.\n", __FL__);
        if (head->magic_num == MAGIC_NUM) {
			ret = g_svr->persistent->handler(head);

		} else {
			printf("FATAL: [%s][%d] persistent handle msg error [%d].\n", __FL__, ret);
		}
		free(head);

		usleep(SLEEPTIME);
    }
    
    return NULL;
}

static void *__middle_routine(void *ctx)
{
    thread_begin("middle");
    shield_head_t *head;
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
					printf("ERROR: [%s][%d] middle handle in message error [%d].\n", __FL__, ret);
			} else {
					printf("ERROR: [%s][%d] read from middle in error.\n", __FL__);
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
					printf("ERROR: [%s][%d] middle handle out message error [%d].\n", __FL__, ret);
			} else {
				printf("ERROR: [%s][%d] read from core out message error [%d].\n", __FL__, ret);
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
    pthread_create(&tp->persistent_tid, NULL, __persistent_routine, NULL);
    pthread_create(&tp->middle_tid, NULL, __middle_routine, NULL);
}

static void __join()
{
    pthread_join(tp->manage_tid, NULL);
    pthread_join(tp->read_tid, NULL);
    pthread_join(tp->write_tid, NULL);
    pthread_join(tp->core_tid, NULL);
    pthread_join(tp->persistent_tid, NULL);
    pthread_join(tp->middle_tid, NULL);
}


thread_pool_t *thread_pool_init()
{
    tp = calloc(1, sizeof(thread_pool_t));

    tp->run = __run;
    tp->join = __join;

    if ((tp->read_in = queue_init()) == NULL) {
        printf("[%s][%d] FATAL: queue init error.\n", __FL__);
        return NULL;
    }
    if ((tp->read_out = queue_init()) == NULL) {
        printf("[%s][%d] FATAL: queue init error.\n", __FL__);
        return NULL;
    }
    if ((tp->write_in = queue_init()) == NULL) {
        printf("[%s][%d] FATAL: queue init error.\n", __FL__);
        return NULL;
    }
    if ((tp->middle_in = queue_init()) == NULL) {
        printf("[%s][%d] FATAL: queue init error.\n", __FL__);
        return NULL;
    }
    if ((tp->middle_out = queue_init()) == NULL) {
        printf("[%s][%d] FATAL: queue init error.\n", __FL__);
        return NULL;
    }
    if ((tp->core_in = queue_init()) == NULL) {
        printf("[%s][%d] FATAL: queue init error.\n", __FL__);
        return NULL;
    }
    if ((tp->core_out = queue_init()) == NULL) {
        printf("[%s][%d] FATAL: queue init error.\n", __FL__);
        return NULL;
    }
    if ((tp->persistent_in = queue_init()) == NULL) {
        printf("[%s][%d] FATAL: queue init error.\n", __FL__);
        return NULL;
    }

    return tp;
}
