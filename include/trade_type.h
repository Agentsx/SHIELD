#ifndef __TRADE_TYPE_H__
#define __TRADE_TYPE_H__

#define SSE      99
#define SZSE     98

#define MAX_BIZ_CMD    0x7F

/* out cmd 0x01~0x40 */
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

/* system cmd 0x41~0x60*/
#define CMD_ADD_FD         0x41
#define CMD_DEL_FD         0x42
#define CMD_CLOCK_MSG      0x43

/* persistent cmd 0x61~0xA0*/
#define CMD_UDPATE_TRADE_VOL         0x61
#define CMD_INSERT_TRADE_INF         0x62
#define CMD_UPDATE_CLIENT_QUANTITY   0x63

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
