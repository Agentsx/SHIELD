#include "core.h"
#include "middle/middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include <stdio.h>
#include <string.h>	
#include <time.h> 
#include <sys/time.h> 


static int __package_head(msg_head_t *h)
{
	h->msg_len = LOGOUT_RSP_LEN; 
	h->fix_length = NONFIX; 
	h->rec_length = LOGOUT_RSP_BODY_LEN; 
	h->rec_no = 1; 
	strncpy(h->msg_type, S212, sizeof(h->msg_type));
	h->trans_no = 0; 
	h->signature_flag = NONSIGNATURED; 
	h->encrypted = NONENCRYTED; 
	h->resend_flag = 0; 
	strncpy(h->reserved, "123", sizeof(h->reserved)); 
	strncpy(h->signature_data, "123", sizeof(h->signature_data)); 
	return 0;
}

int ping_req_handler(shield_head *h)
{
	printf("TRACE: [%s][%d] ping handler called.\n", __FL__);
	
	CALLOC_MSG(ping_req, h->fd, PING_REQ);

	__package_head(&ping_req->msg_head);
	
	printf("TRACE: [%s][%d] ping head package ok.\n", __FL__);

	struct timeval timenow;
    gettimeofday( &timenow, NULL );
	int now_time_ms=timenow.tv_usec/10000;

	time_t timep;  
	struct tm *p;  
	time(&timep);  
	p =localtime(&timep);

	sprintf(ping_req->date_time,"%d%d%d%d%d%d%d",\
		 1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,now_time_ms);
	strncpy(ping_req->description, "ping req!", sizeof(ping_req->description));
		
	printf("TRACE: [%s][%d]ping rsp body package ok.\n", __FL__);
	printf("TRACE: [%s][%d] push to middle[%p].\n", __FL__, g_svr->core->push_to_middle);
	PUSH_MSG(ping_req);

	return 0;
}

int ping_rsp_handler(shield_head *h)
{
	printf("TRACE: [%s][%d] ping handler called.\n", __FL__);


	
	printf("TRACE: [%s][%d]ping rsp body package ok.\n", __FL__);
	printf("TRACE: [%s][%d] push to middle[%p].\n", __FL__, g_svr->core->push_to_middle);
	PUSH_MSG(ping_rsp);

	return 0;
}
