#include "protocol.h"
#include "utils/log.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static int __readn(int fd, void *buff, size_t n)
{
	size_t  nleft;
	ssize_t nread;
	char   *ptr;

	ptr = buff;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		} else if (nread == 0)
			break;

		nleft -= nread;
		ptr += nread;
	}
	return n - nleft;
}

static void *__pro_read(int fd, size_t *len)
{
	char slen[LEN_OF_MSGLEN + 1] = {0};
	int ret;
	ret = __readn(fd, slen, LEN_OF_MSGLEN);
	if (ret != LEN_OF_MSGLEN) {
		log_error("read length of package error.");
		return NULL;
	}
                
	int ilen;
	ilen = atoi(slen);
	if (ilen <= 0) {
		log_error("read length of package slen[%s], ilen[%d].", slen, ilen);
		return NULL;
	}

	void *msg = calloc(ilen + 1, sizeof(char));
	memcpy(msg, slen, LEN_OF_MSGLEN);
	ret = __readn(fd, msg + LEN_OF_MSGLEN, ilen - LEN_OF_MSGLEN);
	if (ret != ilen - LEN_OF_MSGLEN) {
		log_error("read package ilen[%d] error ret[%d].", ilen - LEN_OF_MSGLEN, ret);
		free(msg);
		return NULL;
	}
	
	*len = ilen + 1;
	return msg;	
}

static int __writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return n - nleft;
}

static int __pro_write(int fd, void *msg, size_t len)
{
	int ret = __writen(fd, msg, len);
	return ret == len ? 0 : -1;
}

protocol_t *init_protocol()
{
	protocol_t *p = calloc(1, sizeof(protocol_t));

	p->pro_read = __pro_read;
	p->pro_write = __pro_write;

	return p;
}
