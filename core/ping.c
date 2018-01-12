#include "core.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "utils/log.h"
#include "utils/log.h"

#include <string.h>	
#include <time.h> 
#include <sys/time.h> 


static int __package_ping_head(msg_head_t *h, const char *type, int msg_len, int body_len)
{
	h->msg_len = msg_len; 
	h->fix_length = NONFIX; 
	h->rec_length = body_len; 
	h->rec_no = 1;
	strncpy(h->msg_type, type, sizeof(h->msg_type));
	h->trans_no = 0;
	h->signature_flag = NONSIGNATURED;
	h->encrypted = NONENCRYTED;
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 
	return 0;
}

int ping_req_handler(shield_head_t *h)
{
	log_notice("==ping handler begin==");

    ping_req_t *ping_req = (ping_req_t *)(h + 1);
	CALLOC_MSG(ping_rsp, h->fd, CMD_PING_RSP, 0);

	__package_ping_head(&ping_rsp->msg_head, MT_PING_RSP, PING_RSP_BODY_LEN + MSG_HEAD_LEN, PING_RSP_BODY_LEN);

	STRNCPY(ping_rsp->date_time, ping_req->date_time);
	STRNCPY(ping_rsp->description, ping_req->description);
		
	PUSH_PING_RSP(ping_rsp);
	log_notice("==ping handler end==");

	return 0;
}

int ping_rsp_handler(shield_head_t *h)
{ // nothing to do
	return 0;
}

int send_ping(int fd)
{
    log_notice("==send ping begin==");
    CALLOC_MSG(ping_req, fd, CMD_PING_REQ, 0);

    __package_ping_head(&ping_req->msg_head, MT_PING_REQ, PING_REQ_BODY_LEN+MSG_HEAD_LEN, PING_REQ_BODY_LEN);

	struct timeval timenow;
    gettimeofday( &timenow, NULL );

	time_t timep;  
	struct tm *p;  
	time(&timep);  
	p = localtime(&timep);

	sprintf(ping_req->date_time, "%04d%02d%02d%02d%02d%02d%02d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, 99);
	STRNCPY(ping_req->description, "ping req!");

	PUSH_MSG(ping_req);
    log_notice("==send ping end==");
    return 0;
}
