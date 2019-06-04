#!/usr/bin/env python2

import socket
import paramiko
import threading
import sys

sys.path.append("../swig/")
import kalashnikovDB as ak47
import sql_executor as sqle
import server_functions as srv
#Refactored and documented by Marko Vertus
#Creates a threaded wrapper
def Threaded(fn):
    def wrapper(*args, **kwargs):
        t = threading.Thread(target=fn, args=args, kwargs=kwargs)
        t.start()
    return wrapper
#Main class of the server.py
class Server:
    #Executor for executing sql commands
    executor = sqle.sql_executor()
           
    #Constructor of Server class
    def __init__(self, host="localhost", port=1998):
        self.host = host;
        self.port = port;
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.transport = None
        self.channel = None
        self.working = True
        print self.host
        print self.port
    #Destructor of Server class
    def __del__(self):
        self.working = False
        self.sock.close()
    #Functions that starts the server by binding the socket to the IP and port
    def start(self):
        try:
            self.sock.bind((self.host, self.port))
        except Exception, e:
            print "[-] Failed to start server: %s" %e
            self.__del__()

        self.sock.listen(1)
        
        while self.working:
            print "Awaiting connection"
            conn, addr = self.sock.accept()
            print "[*] Incoming connection from %s" %addr[0]
            ssh_conn = srv.Connection(conn, addr)
            self.handle_connection(ssh_conn)
    #Function for handling connections and also disconnects from the server
    @Threaded
    def handle_connection(self, conn):
        while conn.is_alive():
            cmd = conn.recv_data()
            out = Server.process_command(cmd)
            conn.send_data(out)   
        print "[*] Client from %s disconnected from server" % conn.addr[0]
        del conn
    #Function for executing SQL commands via sql.executor()
    @staticmethod
    def process_command(cmd):
        try:
            out = Server.executor.execute(cmd)
        except Exception, e:
            err = "[-] Failed command execution: %s" %e
            out = err
            print err
        return out
# Starts up the server
s = Server()
s.start()

