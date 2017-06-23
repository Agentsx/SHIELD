#include "core.h"
#include "include/trade_type.h"

handler_map_t g_hp[] = {
	{CMD_LOGIN_REQ, login_req_handler},
	{CMD_BIZ_OVER_REQ, biz_over_req_handler}, 
	{CMD_LOGOUT_REQ, logout_req_handler},
	{CMD_PING_REQ, ping_req_handler},
	{CMD_PING_RSP, ping_rsp_handler},
	{CMD_ADD_VOL_REQ, add_vol_req_handler},
	{CMD_CUT_VOL_REQ, cut_vol_req_handler},
	{CMD_TRADE_QRY_REQ, trade_qry_req_handler},
    {0, NULL}
};
