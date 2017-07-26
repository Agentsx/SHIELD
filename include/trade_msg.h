#ifndef __TRADE_MSG_H_
#define __TRADE_MSG_H_

#define MSG_HEAD_LEN  180

#define  LOGIN_REQ_BODY_LEN         66
#define  LOGIN_RSP_BODY_LEN         63
#define  BIZ_OVER_REQ_BODY_LEN  	20
#define  BIZ_OVER_RSP_BODY_LEN  	20
#define  LOGOUT_REQ_BODY_LEN        65
#define  LOGOUT_RSP_BODY_LEN   		0
#define  PING_REQ_BODY_LEN  		24
#define  PING_RSP_BODY_LEN  		24
#define  ADD_VOL_REQ_BODY_LEN       54
#define  ADD_VOL_RSP_BODY_LEN    	99
#define  CUT_VOL_REQ_BODY_LEN       54
#define  CUT_VOL_RSP_BODY_LEN       99
#define  TRADE_QRY_REQ_BODY_LEN     16
#define  TRADE_QRY_RSP_BODY_LEN    	66

#define  NONFIX         0
#define  FIX            1
#define  NONSIGNATURED  0
#define  SIGNATURED     1
#define  NONENCRYTED    0
#define  ENCRYTED       1

typedef struct msg_head_s {
	long long   msg_len;
	long long   fix_length;
	long long   rec_length;
	long long   rec_no;
	char        msg_type[8];
	long long   trans_no;
	long long   signature_flag;
	long long   encrypted;
	long long   resend_flag;
	char        reserved[16];
	char        signature_data[136];
} msg_head_t;

typedef struct login_req_s {
	msg_head_t  msg_head;
	char        user_name[16];
	char        password[24];
	long long   heart_bt_int;
	char        data_date[16];
	long long   begin_trans_no;
	char        connection_type[8];
} login_req_t;

typedef struct login_rsp_s {
	msg_head_t  msg_head;
	char        result[8];
	long long   heart_bt_int;
	char        data_date[16];
	long long   begin_trans_no;
	char        description[24];
	char        connection_type[8];
} login_rsp_t;

typedef struct biz_over_req_s {
	msg_head_t  msg_head;
	char 		biz_code[8];
	long long	total_records;
	char		tran_status[8];

} biz_over_req_t;

typedef struct biz_over_rsp_s {
	msg_head_t  msg_head;
	char 		biz_code[8];
	long long	total_records;
	char		tran_status[8];

} biz_over_rsp_t;

typedef struct logout_req_s {
	msg_head_t  msg_head;
	char 		code[8];
	char 		description[72];
} logout_req_t;

typedef struct logout_rsp_s {
	msg_head_t msg_head;
} logout_rsp_t;

typedef struct ping_req_s {
	msg_head_t  msg_head;
	char 		date_time[24];
	char		description[16];
} ping_req_t;

typedef struct ping_rsp_s {
	msg_head_t  msg_head;
	char 		date_time[24];
	char		description[16];
} ping_rsp_t;

typedef struct add_vol_req_s{
	msg_head_t  msg_head;
	char		instruction_id[24];
	char		instrument_id[8];
	char		account_id[16];
	char		PBU[8];
	long long	quantity;
} add_vol_req_t;

typedef struct add_vol_rsp_s{
	msg_head_t  msg_head;
	char		processing_result[8];
	char		description[40];
	char		org_instruction_id[16];
	char		instrument_id[8];
	char		account_id[16];
	char		PBU[8];
	long long	quantity;
} add_vol_rsp_t;

typedef struct cut_vol_req_s{
	msg_head_t  msg_head;
	char		instruction_id[24];
	char		instrument_id[8];
	char		account_id[16];
	char		PBU[8];
	long long	quantity;
} cut_vol_req_t;

typedef struct cut_vol_rsp_s{
	msg_head_t  msg_head;
	char		processing_result[8];
	char		description[40];
	char		org_instruction_id[16];
	char		instrument_id[8];
	char		account_id[16];
	char		PBU[8];
	long long 	quantity;
} cut_vol_rsp_t;

typedef struct trade_qry_req_s{
	msg_head_t  msg_head;
	char	    org_instruction_id[24];
} trade_qry_req_t;

typedef struct trade_qry_rsp_s{
	msg_head_t  msg_head;
	char	    processing_result[8];
	char	    org_instruction_id[24];
	char	    org_processing_result[8];
	char 	    org_description[40];
} trade_qry_rsp_t;

#endif
