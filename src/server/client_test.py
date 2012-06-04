#client_test.py Tests comumnication with server

import socket

ip="localhost"
port=1999
bufferSize=1024

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((ip, port))
print sock.recv(bufferSize)
sock.send('bla')
print sock.recv(bufferSize)
sock.send('bli')
print sock.recv(bufferSize)
sock.send('blu')
print sock.recv(bufferSize)
sock.send('disconnect')
