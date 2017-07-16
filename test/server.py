#-*-coding:utf-8 -*-
import socket 
ip_port = ("127.0.0.1",9999)    #服务器端ip和服务端口
server = socket.socket()            #创建server 
server.bind(ip_port)                #绑定地址ip
server.listen(5)                    #开始监听，允许5个客户端排队
conn,addr = server.accept()         #等待连接
while True:
	client_data = conn.recv(1024)       #收取信息，1024字节
	if client_data == 'exit':
		print (client_data)
		conn.sendall(client_data.upper())
		break
	print(client_data)
	conn.sendall(client_data)           #发送数据
  
conn.close()                        #关闭程序`kk
