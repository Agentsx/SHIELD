#ifndef __TRADE_MSG_H_
#define __TRADE_MSG_H_

#define  LOGOUT_RSP_LEN      243
#define  LOGOUT_RSP_BODY_LEN 63

#define  NONFIX 0
#define  FIX    1
#define  NONSIGNATURED  0
#define  SIGNATURED     1
#define  NONENCRYTED    0
#define  ENCRYTED       1

typedef struct msg_head_s {
	long long msg_len;
	long long fix_length;
	long long rec_length;
	long long rec_no;
	char      msg_type[8];
	long long trans_no;
	long long signature_flag;
	long long encrypted;
	long long resend_flag;
	char      reserved[16];
	char      signature_data[136];
} msg_head_t;

typedef struct login_req_s {
	msg_head_t msg_head;
	char       user_name[16];
	char       password[24];
	long long  heart_bt_int;
	char       data_date[16];
	long long  begin_trans_no;
	char       connection_type[8];
} login_req_t;

typedef struct login_rsp_s {
	msg_head_t msg_head;
	char       result[8];
	long long  heart_bt_int;
	char       data_date[16];
	long long  begin_trans_no;
	char       description[24];
	char       connection_type[8];
} login_rsp_t;

typedef struct biz_over_rsp_s {
	msg_head_t msg_head;
	char 		bizCode[3];
	long long	totalRecords;
	char		tranStatus;

} biz_over_rsp_t;
typedef struct logout_rsp_s {
	msg_head_t msg_head;
} logout_rsp_t;

typedef struct ping_req_s {
	msg_head_t msg_head;
	char 		dateTime[16];
	char		description[8];
} ping_req_t;

typedef struct ping_rsp_s {
	msg_head_t msg_head;
	char 		dateTime[16];
	char		description[8];
} ping_rsp_t;

typedef struct add_vol_rsp_s{
	msg_head_t msg_head;
	char	processingResult[5];
	char	description[40];
	char	orgInstructionId[16];
	char	instrumentId[6];
	char	accountId[10];
	char	PBU[6];
	char	quantity[16];

}add_vol_rsp_t;

typedef struct cut_vol_rsp_s{
	msg_head_t msg_head;
	char	processingResult[5];
	char	description[40];
	char	orgInstructionId[16];
	char	instrumentId[6];
	char	accountId[10];
	char	PBU[6];
	char	quantity[16];

}cut_vol_rsp_t;

typedef struct trade_qry_rsp_s{
	msg_head_t msg_head;
	char	processingResult[5];
	char	orgInstructionId[16];
	char	orgProcessingResult[5];
	char 	orgDescription[40];
}trade_qry_rsp_t;

#endif
