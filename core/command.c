#include "core.h"
#include "include/trade_type.h"

handler_map_t g_hp[] = {
	{LOGIN_REQ, login_req_handler},
	{BIZ_OVER_REQ, biz_over_req_handler}, 
	{LOGOUT_REQ, logout_req_handler},
	{PING_REQ, ping_req_handler},
	{PING_RSP, ping_rsp_handler},
	{ADD_VOL_REQ, add_vol_req_handler},
	{CUT_VOL_REQ, cut_vol_req_handler},
	{TRADE_QRY_REQ, trade_qry_req_handler},
    {0, NULL}
};
