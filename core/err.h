#ifndef __ERR_H__
#define __ERR_H__

/* login error code. begin*/
#define USER_NOT_FOUND        			("1", "user not found")
#define WRONG_PASSWORD         			("1", "wrong password")
#define USER_STATUS_ERR       		 	("1", "user status error")
#define TRADE_DATE_ERR         			("1", "trade date error")
/* end */

#define LOGIN_SUCCESS          		("0", "login ok")
#define TRADE_SUCCESS          		("00001", "trade ok")
#define ETF_CODE_NOT_FOUND     		("00002", "etf code not found")
#define CLIENT_NOT_FOUND       		("00003", "client not found")
#define CLIENT_NOT_INUSE      	 	("00004", "client not in use")
#define PBU_ERROR              		("00005", "pbu error")
#define SO_BAD                 		("00006", "Internal fault")
#define BEYOND_APPLY_LIMIT     		("00007", "beyond apply limit")
#define INSTRUCTION_HANDLED    		("00008", "instruction handled")

#define REDEMPTION_SUCCESS     		("00009", "redemption ok")
#define BEYOND_REDEMPTION_LIMIT     ("00010", "beyond redemption limit")

#define APPLY_LIMIT_NOT_FOUND 			("00011", "apply limit not found")
#define REDEMPTION_LIMIT_NOT_FOUND	 	("00012", "redemption limit not found")
#define QUANTITY_ERROR         			("00013", "quantity is over range")
#define TRADE_TIME_ERR         			("00014", "not in trade time now")



#endif
