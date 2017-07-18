#!/usr/bin/python
# -*- coding: UTF-8 -*-
import socket
import time
import sys
import json

from package_conf import *

def get_pkg(pkg_name):
	if pkg_name in package_conf:
		conf = package_conf[pkg_name]
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
		server_data = self.client.recv(1024)
		print (server_data.decode("utf-8"))
	def _heart_beat_(self):
		while True:
			self.client.send('hello!\r\n'.encode()) 
			server_data = self.client.recv(1024)
			print(server_data.decode("utf-8"))
			time.sleep(5)

if __name__ == '__main__':
	## package = get_pkg(sys.argv[1])
	#for pkg in package_conf:
	#	conf = package_conf[pkg]
	#	pkg_content = ''
	#	for i in conf:
	#		pkg_content = pkg_content + str(i)

	#	obj = Sending(pkg_content)
	#	obj._send_pkg_()
	#	print("send pkg", pkg_content)
	#	# obj._heart_beat_()
	pkg_name = ['login_req', 'addvol_req', 'cutvol_req', 'logout_req']
	obj = Sending()
	for name in pkg_name:
		package = get_pkg(name)
		obj._send_pkg_(package)
		print("send pkg", package)


