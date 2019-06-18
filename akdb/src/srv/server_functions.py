#!/usr/bin/env python2

import socket
import paramiko
import threading
import sys

sys.path.append("../swig/")
import kalashnikovDB as ak47
import sql_executor as sqle

#Interface to override classic python server support
class ParamikoServer(paramiko.ServerInterface):
    def __init__(self):
        self.event = threading.Event()
    #Function that checks if the channel can be opened for a requesting client
    def check_channel_request(self, kind, chanid):
        if kind == 'session':
            return paramiko.OPEN_SUCCEEDED
        return paramiko.OPEN_FAILED_ADMINISTRATIVELY_PROHIBITED
    #Function that checks if the clients username and password match
    def check_auth_password(self, username, password):
        usr = "testingUser" #TODO get users and passwords from database
        #Testing the data format of AK_user_get_id(usr)
        #SIGSEGV ISSUE again
        #return ak47.AK_user_get_id(usr)
        pas = "testingPass"
        #local login using swig

    #    if(ak47.AK_user_get_id(usr)!=null):
    #       return paramiko.AUTH_SUCCESSFUL
    #    elif(username == usr) and (password == pas):
    #        return paramiko.AUTH_SUCCESSFUL
    #         return paramiko.AUTH_FAILED
        pas = "testingPass"
        if (username == usr) and (password == pas):
            return paramiko.AUTH_SUCCESSFUL
        return paramiko.AUTH_FAILED
#Class that handles connection from client to the server
class Connection:
    #Constructor of the class 
    def __init__(self, conn, addr):
        self.addr = addr
        self.transport = paramiko.Transport(conn)
        self.transport.add_server_key(paramiko.RSAKey.generate(2048))
        self.transport.start_server(server=ParamikoServer())
        self.channel = self.transport.accept(timeout=1)
    #Destructor of the class
    def __del__(self):
        if self.channel is not None:
            self.channel.close()
        if self.transport is not None:
            self.transport.close()
    #Function that handles sending data to the client
    def send_data(self, data):
        #TODO implement protocol
        try:
            self.channel.send(self.pack_output(data))
        except Exception, e:
            print "[-] Failed sending data to client: %s" %e
    #Function that handles recieving data from the client
    def recv_data(self):
        #TODO implement protocol
        return self.unpack_input(self.channel.recv(1024))
    #Function that formats data into a JSON format
    def pack_output(self, out):
        #TODO pack into json
        return out
    #Fucntion that reads JSON format and converts it to XY
    def unpack_input(self, inp):
        #TODO unpack json
        inp = inp.strip()
        return inp
    #Function that checks if the server is still running
    def is_alive(self):
        if self.channel is not None and self.transport.is_active():
            return True
        return False