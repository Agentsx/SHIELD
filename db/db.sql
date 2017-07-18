drop table t_user;
create table t_user (
	f_username varchar(40) primary key,
	f_password varchar(256) not null,
	f_status   int not null
);

drop table t_client;
create table t_client (
	f_acc_no   varchar(20) primary key,
	f_pbu      varchar(10) not null,
	f_quantity bigint not null,
	f_status   int    not null
);

drop table t_trade_info;
create table t_trade_info (
	f_trade_date  varchar(8),
	f_sge_instruc varchar(16),
	f_recv_type   int,
	f_trans_no    int not null,
	f_msg_type    int not null,
	f_etf_code    varchar(8) not null,
	f_client_acc  varchar(20) not null,
	f_pbu         varchar(10) not null,
	f_quantity    bigint not null,
	f_result_code varchar(10),
	f_result_desc varchar(80),
	primary key(f_trade_date, f_sge_instruc, f_recv_type)
);

drop table t_trade_date;
create table t_trade_date (
	f_trade_date  varchar(8) not null
);

drop table t_trade_time;
create table t_trade_time (
	f_seq        int primary key,
	f_start_time varchar(8) not null,
	f_end_time   varchar(8) not null
);

drop table t_trade_list;
create table t_trade_list (
	f_trade_date  varchar(8) not null,
	f_etf_code    varchar(10) not null,
	f_apply_limit bigint not null,
	f_redemp_limit bigint not null,
	primary key(f_trade_date, f_etf_code)
);

drop table t_trade_vol;
create table t_trade_vol (
	f_trade_date   varchar(8) not null,
	f_etf_code     varchar(10) not null,
	f_apply        bigint,
	f_redemption   bigint,
	primary key(f_trade_date, f_etf_code)
);
