#ifndef __ERR_H__
#define __ERR_H__

#define USER_NOT_FOUND         ("1", "user not found")
#define WRONG_PASSWORD         ("1", "wrong password")
#define USER_STATUS_ERR        ("1", "user status error")
#define TRADE_DATE_ERR         ("1", "trade date error")
#define APPLY_LIMIT_NOT_FOUND  ("1", "apply limit not found")
#define QUANTITY_ERROR         ("1", "quantity is over range")
#define LOGIN_SUCCESS          ("0", "login ok")
#define TRADE_SUCCESS          ("00001", "trade ok")
#define ETF_CODE_NOT_FOUND     ("00002", "etf code not found")
#define CLIENT_NOT_FOUND       ("00003", "client not found")
#define CLIENT_NOT_INUSE       ("00004", "client not in use")
#define PBU_ERROR              ("00005", "pbu error")
#define SO_BAD                 ("00006", "Internal fault")
#define BEYOND_APPLY_LIMIT     ("00007", "beyond apply limit")
#define INSTRUCTION_HANDLED    ("00008", "instruction handled")

#endif
