#ifndef __MIDDLE_H__
#define __MIDDLE_H__

#include "frame/frame.h"

#define MSG_HEAD_LEN  180

#define LOGIN_RSP_BODY_LEN  		63
#define BIZ_OVER_RSP_BODY_LEN  		20
#define LOGOUT_RSP_BODY_LEN   		0
#define PING_REQ_BODY_LEN  			24
#define PING_RSP_BODY_LEN  			24
#define ADD_VOL_RSP_BODY_LEN    	99
#define CUT_VOL_RSP_BODY_LEN    	99
#define TRADE_QRY_RSP_BODY_LEN    	72



typedef struct template_s {
	int  len_in_msg;
	char type;
	int  len_in_struct;
} template_t;


#define TRIM(dest, n) \
    do { \
		char *p = dest + n - 1;\
		while (*p == ' ') { *p = 0; p--; } \
	} while (0)

#define STRTRIMNCPY(dest, src, n) \
		strncpy(dest, src, n); \
		TRIM(dest, n); 

#define MIDDLE_PUSH_IN(h) \
	g_svr->middle->push_in(h); 

#define MIDDLE_PUSH_OUT(h) \
	g_svr->middle->push_out(h); 



extern int resolve_msg(shield_head_t *head);
extern int package_msg(shield_head_t *head);

#endif
