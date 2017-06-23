#ifndef __TRADE_TYPE_H__
#define __TRADE_TYPE_H__

#include "frame/frame.h"

#define MAX_BIZ_CMD    0x7F

#define LOGIN_REQ      0x01
#define LOGIN_RSP      0x02
#define BIZ_OVER_REQ   0x03
#define BIZ_OVER_RSP   0x04
#define LOGOUT_REQ     0x05
#define LOGOUT_RSP     0x06
#define PING_REQ       0x07
#define PING_RSP       0x08
#define ADD_VOL_REQ    0x09
#define ADD_VOL_RSP    0x0A
#define CUT_VOL_REQ    0x0B
#define CUT_VOL_RSP    0x0C
#define TRADE_QRY_REQ  0x0D
#define TRADE_QRY_RSP  0x0E
#define ADD_FD         0xFD
#define DEL_FD         0xFE
#define CLOCK_MSG      0xFF

#define S201  "S201"
#define S202  "S202"
#define S205  "S205"
#define S206  "S206"
#define S209  "S209"
#define S210  "S210"
#define S211  "S211"
#define S212  "S212"
#define A301  "A301"
#define A302  "A302"
#define A303  "A303"
#define A304  "A304"
#define A305  "A305"
#define A306  "A306"

#endif
