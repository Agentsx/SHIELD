#!/usr/bin/python
#-*- coding: UTF-8 -*-

IP_ADDRESS = "127.0.0.1"
PORT = 9999

package_conf={
    "login_req":[
        "247   ",	#"msg_len[6]
        "1",		#fix_length[1]
        "67    ",	#rec_length[6]
        "0     ",	#recv_no
        "S201",	#msg_type
        "0               ",#trans_no
        "0",#signature_flag
        "0",#encrypted
        "0",#resend_flag
        "123       ",#reserved
        "                                                                                                                                ",#signaturedata
        "abc       ",#user_name
        "1234567890123456",#password
        "10              ",#heart_beat
        "20170707",#data_dete
        "0               ",#begin_trans_no
        "G"#connection_:type
    ],

	"ping_req": [
        "220   ",#"msg_len[8]
        "1",#fix_length,
        "40    ",#rec_length
        "0     ",#recv_no
        "S211",#msg_type
        "0               ",#trans_no
        "0",#signature_flag
        "0",#encrypted
        "0",#resend_flag
        "123       ",#reserved
        "                                                                                                                                ",#signaturedata
        "2017071914132346        ",#date_time
        "ping_req        "#decription
    ],
	
    "addvol_req": [
        "234   ",#"msg_len[8]
        "1",#fix_length,
        "54    ",#fix_length
        "0     ",#recv_no
        "A301",#msg_type
        "18              ",#trans_no
        "0",#signature_flag
        "0",#encrypted
        "0",#resend_flag
        "123       ",#reserved
        "                                                                                                                                ",#signaturedata
        "ETF1707130000018",# instruction_id
        "518880",# instrument_id
        "123456    ",# account_id
        "32156 ",# PUB
        "300000          "# quantity
    ],
    "cutvol_req": [
        "234   ",#"msg_len[8]
        "1",#fix_length,
        "54    ",#fix_length
        "0     ",#recv_no
        "A303",#msg_type
        "9               ",#trans_no
        "0",#signature_flag
        "0",#encrypted
        "0",#resend_flag
        "123       ",#reserved
        "                                                                                                                                ",#signaturedata
        "ETF1707130000009",# instruction_id
        "518880",# instrument_id
        "123456    ",# account_id
        "32156 ",# PUB
        "300000          "# quantity
    ],
    "qry_req": [
        "196   ",#"msg_len[8]
        "1",#fix_length,
        "16    ",#rec_length
        "0     ",#recv_no
        "A305",#msg_type
        "10              ",#trans_no
        "0",#signature_flag
        "0",#encrypted
        "0",#resend_flag
        "123       ",#reserved
        "                                                                                                                                ",#signaturedata
        "ETF1707130000010"# org_instruction_id///////////////////////
    ],
	"bizover_req": [
        "200   ",#"msg_len[8]
        "1",#fix_length,
        "20    ",#rec_length
        "0     ",#recv_no
        "S205",#msg_type
        "0               ",#trans_no
        "0",#signature_flag
        "0",#encrypted
        "0",#resend_flag
        "123       ",#reserved
        "                                                                                                                                ",#signaturedata
        "001",#biz_code
        "3000            ",#total_records////////////////////
        "0"#trans_status
    ],
	"logout_req": [
        "245   ",#"msg_len[8]
        "1",#fix_length,
        "65    ",#rec_length
        "0     ",#recv_no
        "S209",#msg_type
        "0               ",#trans_no
        "0",#signature_flag
        "0",#encrypted
        "0",#resend_flag
        "123       ",#reserved
        "                                                                                                                                ",#signaturedata
        "0",#code
        "logout                                                          "#description
    ]
}

