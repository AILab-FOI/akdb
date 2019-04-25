#!/usr/bin/env python2

import socket
import paramiko
import threading
import sys

sys.path.append("../swig/")
import kalashnikovDB as ak47
import sql_executor as sqle


def Threaded(fn):
    def wrapper(*args, **kwargs):
        t = threading.Thread(target=fn, args=args, kwargs=kwargs)
        t.start()
    return wrapper


class ParamikoServer(paramiko.ServerInterface):
    def __init__(self):
        self.event = threading.Event()
    def check_channel_request(self, kind, chanid):
        if kind == 'session':
            return paramiko.OPEN_SUCCEEDED
        return paramiko.OPEN_FAILED_ADMINISTRATIVELY_PROHIBITED
    def check_auth_password(self, username, password):
        usr = "testingUser" #TODO get users and passwords from database
        pas = "testingPass"
        if (username == usr) and (password == pas):
            return paramiko.AUTH_SUCCESSFUL
        return paramiko.AUTH_FAILED

class Connection:
    def __init__(self, conn, addr):
        self.addr = addr
        self.transport = paramiko.Transport(conn)
        self.transport.add_server_key(Server.rsa_key)
        self.transport.start_server(server=ParamikoServer())
        self.channel = self.transport.accept(timeout=1)

    def __del__(self):
        if self.channel is not None:
            self.channel.close()
        if self.transport is not None:
            self.transport.close()

    def send_data(self, data):
        #TODO implement protocol
        try:
            self.channel.send(self.pack_output(data))
        except Exception, e:
            print "[-] Failed sending data to client: %s" %e

    def recv_data(self):
        #TODO implement protocol
        return self.unpack_input(self.channel.recv(1024))

    def pack_output(self, out):
        #TODO pack into json
        return out

    def unpack_input(self, inp):
        #TODO unpack json
        inp = inp.strip()
        return inp

    def is_alive(self):
        if self.channel is not None and self.transport.is_active():
            return True
        return False

class Server:

    executor = sqle.sql_executor()
    rsa_key = paramiko.RSAKey.generate(2048)        

    def __init__(self, host="localhost", port=1998):
        self.host = host;
        self.port = port;
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.transport = None
        self.channel = None
        self.working = True

    def __del__(self):
        self.working = False
        self.sock.close()

    def start(self):
        try:
            self.sock.bind((self.host, self.port))
        except Exception, e:
            print "[-] Failed to start server: %s" %e
            self.__del__()

        self.sock.listen(1)
        
        while self.working:
            conn, addr = self.sock.accept()
            print "[*] Incoming connection from %s" %addr[0]
            ssh_conn = Connection(conn, addr)
            self.handle_connection(ssh_conn)

    @Threaded
    def handle_connection(self, conn):
        while conn.is_alive():
            cmd = conn.recv_data()
            out = Server.process_command(cmd)
            conn.send_data(out)   
        print "[*] Client from %s disconnected from server" % conn.addr[0]
        del conn

    @staticmethod
    def process_command(cmd):
        try:
            out = Server.executor.execute(cmd)
        except Exception, e:
            err = "[-] Failed command execution: %s" %e
            out = err
            print err
        return out

s = Server()
s.start()

