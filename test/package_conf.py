#!/usr/bin/python
#-*- coding: UTF-8 -*-
import re

IP_ADDRESS = "127.0.0.1"
PORT = 9999

package_conf={
    "login_req":[
        "241   ",#"msg_len[8]
        "1",#fix_length,
        "61    ",#fix_length
        "0     ",#recv_no
        "S201",#msg_type
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
        "0         ",#begin_trans_no
        "G"#connection_type
    ],
    "logout_req": [
        "245   ",#"msg_len[8]
        "1",#fix_length,
        "65    ",#fix_length
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
    ],
    #"bizover_req": [
    #    "1",#"msg_len[8]
    #    "2   ",#fix_length,
    #    "3",#fix_length
    #    "4",#recv_no
    #    "5",#msg_type
    #    "6",#trans_no
    #    "",#signature_flag
    #    "",#encrypted
    #    "",#resend_flag
    #    "",#reserved
    #    "",#signaturedata
    #    "",#biz_over
    #    "",#total_records
    #    ""#trans_status
    #],
    #"ping_req": [
    #    "1",#"msg_len[8]
    #    "2   ",#fix_length,
    #    "3",#fix_length
    #    "4",#recv_no
    #    "5",#msg_type
    #    "6",#trans_no
    #    "",#signature_flag
    #    "",#encrypted
    #    "",#resend_flag
    #    "",#reserved
    #    "",#signaturedata
    #    "",#date_time
    #    ""#decription
    #],
    #"ping_rsp": [
    #    "1",#"msg_len[8]
    #    "2   ",#fix_length,
    #    "3",#fix_length
    #    "4",#recv_no
    #    "5",#msg_type
    #    "6",#trans_no
    #    "",#signature_flag
    #    "",#encrypted
    #    "",#resend_flag
    #    "",#reserved
    #    "",#signaturedata
    #    "",#date_time
    #    ""#decription
    #],
    #"addvol_req": [
    #    "1",  # "msg_len[8]
    #    "2   ",  # fix_length,
    #    "3",  # fix_length
    #    "4",  # recv_no
    #    "5",  # msg_type
    #    "6",  # trans_no
    #    "",  # signature_flag
    #    "",  # encrypted
    #    "",  # resend_flag
    #    "",  # reserved
    #    "",  # signaturedata
    #    "",# instruction_id
    #    "",# instrument_id
    #    "",# account_id
    #    "",# PUB
    #    ""# quantity
    #],
    #"cutvol_req": [
    #    "1",  # "msg_len[8]
    #    "2   ",  # fix_length,
    #    "3",  # fix_length
    #    "4",  # recv_no
    #    "5",  # msg_type
    #    "6",  # trans_no
    #    "",  # signature_flag
    #    "",  # encrypted
    #    "",  # resend_flag
    #    "",  # reserved
    #    "",  # signaturedata
    #    "",  # instruction_id
    #    "",  # instrument_id
    #    "",  # account_id
    #    "",  # PUB
    #    ""  # quantity
    #],
    #"qry_req": [
    #    "1",  # "msg_len[8]
    #    "2   ",  # fix_length,
    #    "3",  # fix_length
    #    "4",  # recv_no
    #    "5",  # msg_type
    #    "6",  # trans_no
    #    "",  # signature_flag
    #    "",  # encrypted
    #    "",  # resend_flag
    #    "",  # reserved
    #    "",  # signaturedata
    #    ""   # org_instruction_id
    #]
}

