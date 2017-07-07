#ifndef __TBL_H__
#define __TBL_H__

typedef struct user {
    char      name[48];
    char      password[264];
    int       status;
} tbl_user_t;

typedef struct trade_info {
    char      trade_date[16];
    char      sge_instruc[24];
    int       recv_type;
    long long trans_no;
    int       msg_type;
    char      etf_code[16];
    char      client_acc[24];
    char      pbu[16];
    long long quantity;
    char      result_code[8];
    char      result_desc[88];
} tbl_trade_info_t;

typedef struct trade_list {
    char      etf_code[16];
    long long apply_limit;
    long long redemption_limit;
} tbl_trade_list_t;

typedef struct client {
    char      acc_no[24];
    char      pbu[16];
    long long quantity;
    int       status;
} tbl_client_t;

typedef struct trade_vol {
    char      trade_date[16];
    char      etf_code[16];
    long long apply;
    long long redemption;
} tbl_trade_vol_t;

#endif
