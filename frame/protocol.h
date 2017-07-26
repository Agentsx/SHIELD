#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdlib.h>

#define LEN_OF_MSGLEN 6

typedef struct protocol_s {
	int  (*pro_read)(int fd, void **m, size_t *len);
	int  (*pro_write)(int fd, void *msg, size_t len);
} protocol_t;

extern protocol_t *init_protocol();

#endif
