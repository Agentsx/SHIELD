#include "middle.h"
#include "include/trade_type.h"
#include "include/trade_msg.h"

template_t head_template[] = {
	{6, 'L', 0},
	{1, 'L', 0},
	{6, 'L', 0},
	{6, 'L', 0},
	{4, 'S', 8},
	{16, 'L', 0},
	{1, 'L', 0},
	{1, 'L', 0},
	{1, 'L', 0},
	{10, 'S', 16},
	{128, 'S', 136},
	{-1, -1, -1}
};

template_t login_req_template[] = {
	{10, 'S', 16},
	{16, 'S', 24},
	{16, 'L', 0},
	{8, 'S', 16},
	{16, 'L', 0},
	{1, 'S', 8},
	{-1, -1, -1}
};

template_t login_rsp_template[] = {
	{1, 'S', 8},
	{16, 'L', 0},
	{8, 'S', 16},
	{16, 'L', 0},
	{21, 'S', 24},
	{1, 'S', 8},
	{-1, -1, -1}
};

template_t biz_over_req_template[] = {
	{3, 'S', 8},
	{16, 'L', 0},
	{1, 'S', 8},
	{-1, -1, -1}
};

template_t biz_over_rsp_template[] = {
	{3, 'S', 8},
	{16, 'L', 0},
	{1, 'S', 8},
	{-1, -1, -1}
};	

template_t logout_req_template[] = {
	{1, 'S', 8},
	{64, 'S', 72},
	{-1, -1, -1}
};

template_t logout_rsp_template[] = {
	{-1, -1, -1}
};

template_t ping_req_template[] = {
	{16, 'S', 24},
	{8, 'S', 16},
	{-1, -1, -1}
};

template_t ping_rsp_template[] = {
	{16, 'S', 24},
	{8, 'S', 16},
	{-1, -1, -1}
};

template_t add_vol_req_template[] = {
	{16, 'S', 24},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};

template_t add_vol_rsp_template[] = {
	{5, 'S', 8},
	{40, 'S', 48},
	{16, 'S', 24},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};

template_t cut_vol_req_template[] = {
	{16, 'S', 24},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};

template_t cut_vol_rsp_template[] = {
	{5, 'S', 8},
	{40, 'S', 48},
	{16, 'S', 24},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};

template_t trade_qry_req_template[] = {
	{16, 'S', 24},
	{-1, -1, -1}
};

template_t trade_qry_rsp_template[] = {
	{5, 'S', 8},
	{16, 'S', 24},
	{5, 'S', 8},
	{40, 'S', 48},
	{-1, -1, -1}
};

type_mapping_t tm[] = {
    {MT_LOGIN_REQ,     CMD_LOGIN_REQ,     sizeof(login_req_t),     LOGIN_REQ_BODY_LEN,     login_req_template},    
    {MT_LOGIN_RSP,     CMD_LOGIN_RSP,     sizeof(login_rsp_t),     LOGIN_RSP_BODY_LEN,     login_rsp_template},
    {MT_BIZ_OVER_REQ,  CMD_BIZ_OVER_REQ,  sizeof(biz_over_req_t),  BIZ_OVER_REQ_BODY_LEN,  biz_over_req_template}, 
    {MT_BIZ_OVER_RSP,  CMD_BIZ_OVER_RSP,  sizeof(biz_over_rsp_t),  BIZ_OVER_RSP_BODY_LEN,  biz_over_rsp_template}, 
    {MT_LOGOUT_REQ,    CMD_LOGOUT_REQ,    sizeof(logout_req_t),    LOGOUT_REQ_BODY_LEN,    logout_req_template},   
    {MT_LOGOUT_RSP,    CMD_LOGOUT_RSP,    sizeof(logout_rsp_t),    LOGOUT_RSP_BODY_LEN,    logout_rsp_template},   
    {MT_PING_REQ,      CMD_PING_REQ,      sizeof(ping_req_t),      PING_REQ_BODY_LEN,      ping_req_template},     
    {MT_PING_RSP,      CMD_PING_RSP,      sizeof(ping_rsp_t),      PING_RSP_BODY_LEN,      ping_rsp_template},
    {MT_ADDVOL_REQ,    CMD_ADD_VOL_REQ,   sizeof(add_vol_req_t),   ADD_VOL_REQ_BODY_LEN,   add_vol_req_template},  
    {MT_ADDVOL_RSP,    CMD_ADD_VOL_RSP,   sizeof(add_vol_rsp_t),   ADD_VOL_RSP_BODY_LEN,   add_vol_rsp_template},  
    {MT_CUTVOL_REQ,    CMD_CUT_VOL_REQ,   sizeof(cut_vol_req_t),   CUT_VOL_REQ_BODY_LEN,   cut_vol_req_template},  
    {MT_CUTVOL_RSP,    CMD_CUT_VOL_RSP,   sizeof(cut_vol_rsp_t),   CUT_VOL_RSP_BODY_LEN,   cut_vol_rsp_template},
    {MT_TRADE_QRY_REQ, CMD_TRADE_QRY_REQ, sizeof(trade_qry_req_t), TRADE_QRY_REQ_BODY_LEN, trade_qry_req_template},
    {MT_TRADE_QRY_RSP, CMD_TRADE_QRY_RSP, sizeof(trade_qry_rsp_t), TRADE_QRY_RSP_BODY_LEN, trade_qry_rsp_template},
    {"", -1, -1, -1, NULL}
};
