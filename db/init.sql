delete from t_trade_date;
delete from t_trade_list;
delete from t_trade_time;
delete from t_trade_vol;
delete from t_trade_info;

insert into t_trade_date values('20170707');
insert into t_trade_list values('20170707', '518880', 30000000, 3000000);
insert into t_trade_list values('20170707', '518800', 30000000, 3000000);
insert into t_trade_time values(1, '9:30', '11:30');
insert into t_trade_time values(2, '13:30', '15:30');
insert into t_trade_vol values('20170707', '518880', 0, 0);
insert into t_trade_vol values('20170707', '518800', 0, 0);
