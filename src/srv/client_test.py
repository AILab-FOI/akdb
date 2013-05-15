#client_test.py Tests comumnication with server

import socket

welcome_note =  '''Welcome to the AKDB client. Enter your commands in the command line.
To see the list of all commands type '\h' or 'help'
'''

def AK_client_connect(ip, port, buffer_size):

	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((ip, port))

	print welcome_note
	while True:
		response = sock.recv(buffer_size)
		if response != None:
			print "akdb_server> " + response
		command = raw_input("akdb> ")
		if command == 'disconnect':
			print "Exiting now..."
			return
		sock.send(command.strip())


if __name__ == "__main__":
	ip = "localhost"
	port = 1999
	buffer_size = 1024
	AK_client_connect(ip, port, buffer_size)




'''
print sock.recv(bufferSize)
sock.send('bla')
print sock.recv(bufferSize)
sock.send('bli')
print sock.recv(bufferSize)
socket.send('blu')
print sock.recv(bufferSize)
sock.send('disconnect')

Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 43
Server version: 5.5.31-0ubuntu0.12.04.1 (Ubuntu)

Copyright (c) 2000, 2013, Oracle and/or its affiliates. All rights reserved.

Oracle is a registered trademark of Oracle Corporation and/or its
affiliates. Other names may be trademarks of their respective
owners.

Type 'help;' or '\h' for help. Type '\c' to clear the current input statement.

'''