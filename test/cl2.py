#!/usr/bin/python
# -*- coding: UTF-8 -*-
import socket
import time
import sys

from package_conf import *

def get_pkg(pkg_name,i):
	global trans_no
	global instruction_id
	if pkg_name in package_conf:
		conf = package_conf[pkg_name]	
		if pkg_name == 'addvol_req' and i == 0:
			trans_no = int(conf[5])
			instruction_id = int(conf[11][3:])
		elif pkg_name == 'addvol_req' and i > 0:
			trans_no =trans_no+1
			conf[5] = str(trans_no).ljust(16,' ')
		
			instruction_id = instruction_id + 1
			conf[11] =conf[11][:3] + str(instruction_id)
		else:
			pass
		pkg_content = ''
		for i in conf:
			pkg_content = pkg_content + str(i)
		return pkg_content

	else:
		raise Exception(' pkg_name "%s" not found in config' % pkg_name)

class Sending(object):
	def __init__(self):
		self.client=socket.socket()
		self.client.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
		self.client.connect((IP_ADDRESS,PORT))

	def _send_pkg_(self, pkg_content):
		self.client.sendall(pkg_content.encode())
		print("send pkg", package)
	

if __name__ == '__main__':
	pkg_name = ['login_req','ping_req','addvol_req','bizover_req','logout_req']
	obj = Sending()
	trans_no = 0
	instruction_id = 0
	for name in pkg_name:
		if name == 'addvol_req':
			for i in xrange(0,200):
				package = get_pkg('addvol_req',i)
				obj._send_pkg_(package)
		else:
			package = get_pkg(name,0)
			obj._send_pkg_(package)
	
	for i in xrange(0,204):	
		server_data = obj.client.recv(2048)
		print("recv pkg",server_data)
