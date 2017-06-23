#ifndef __TRADE_TYPE_H__
#define __TRADE_TYPE_H__

#define MAX_BIZ_CMD    0x7F

#define CMD_LOGIN_REQ      0x01
#define CMD_LOGIN_RSP      0x02
#define CMD_BIZ_OVER_REQ   0x03
#define CMD_BIZ_OVER_RSP   0x04
#define CMD_LOGOUT_REQ     0x05
#define CMD_LOGOUT_RSP     0x06
#define CMD_PING_REQ       0x07
#define CMD_PING_RSP       0x08
#define CMD_ADD_VOL_REQ    0x09
#define CMD_ADD_VOL_RSP    0x0A
#define CMD_CUT_VOL_REQ    0x0B
#define CMD_CUT_VOL_RSP    0x0C
#define CMD_TRADE_QRY_REQ  0x0D
#define CMD_TRADE_QRY_RSP  0x0E
#define CMD_ADD_FD         0xFD
#define CMD_DEL_FD         0xFE
#define CMD_CLOCK_MSG      0xFF

#define MT_LOGIN_REQ      "S201"
#define MT_LOGIN_RSP      "S202"
#define MT_BIZ_OVER_REQ   "S205"
#define MT_BIZ_OVER_RSP   "S206"
#define MT_LOGOUT_REQ     "S209"
#define MT_LOGOUT_RSP     "S210"
#define MT_PING_REQ       "S211"
#define MT_PING_RSP       "S212"
#define MT_ADDVOL_REQ     "A301"
#define MT_ADDVOL_RSP     "A302"
#define MT_CUTVOL_REQ     "A303"
#define MT_CUTVOL_RSP     "A304"
#define MT_TRADE_QRY_REQ  "A305"
#define MT_TRADE_QRY_RSP  "A306"

#endif
