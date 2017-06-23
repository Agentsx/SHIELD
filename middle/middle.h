#ifndef __MIDDLE_H__
#define __MIDDLE_H__

#include "frame/frame.h"
#include "utils/map.h"

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

extern template_t head_template[];
extern template_t login_req_template[];
extern template_t login_rsp_template[];
extern template_t biz_over_req_template[];
extern template_t biz_over_rsp_template[];
extern template_t logout_req_template[];
extern template_t logout_rsp_template[];
extern template_t ping_req_template[];
extern template_t ping_rsp_template[];
extern template_t add_vol_req_template[];
extern template_t add_vol_rsp_template[];
extern template_t cut_vol_req_template[];
extern template_t cut_vol_rsp_template[];
extern template_t trade_qry_req_template[];
extern template_t trade_qry_rsp_template[];

typedef struct type_mapping_s {
    char       stype[8];
    int        itype;
    int        struct_len;
    int        body_len;
    template_t *templ;
} type_mapping_t;

extern type_mapping_t tm[];
extern map_t *istype_map;  /* key: str, val: int */
extern map_t *type_map;    /* key: int, val: type_mapping_t */

extern int resolve_msg(shield_head_t *head);
extern int package_msg(shield_head_t *head);

#endif
