#!/usr/bin/env python2

import sys
import paramiko

if len(sys.argv) == 3:
    username = sys.argv[1]
    password = sys.argv[2]
else:
    username = raw_input("Username: ")
    password = raw_input("Password: ")

"""
ssh_client = paramiko.SSHClient()
ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

ssh_client.connect(HOST, port=PORT, username=username, password=password)
session = ssh_client.get_transport().open_session()

while True:
    cmd = raw_input("akdb> ")
    session.send(cmd)
    print session.recv(1024)

session.close()
ssh_client.close()
"""

class Client:
    def __init__(self, host="localhost", port=1998):
        self.sock = paramiko.SSHClient()
        self.sock.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.session = None
        self.host = host
        self.port = port
        self.working = True

    def __del__(self):
        self.working = False
        self.session.close()
        self.sock.close()

    def start(self):
        global username
        global password
        connected = False
        
        while not connected:
            self.sock.connect(
                hostname=self.host, 
                port=self.port, 
                username=username, 
                password=password
            )
            connected = True

        self.session = self.sock.get_transport().open_session()

        # receive hello message
        print self.recv_data()
        
        while self.working:
            cmd = raw_input("akdb> ")
            self.send_command(cmd)
            out = self.recv_data()
            print out


    def send_command(self, cmd):
        #TODO implement protocol
        self.session.send(self.pack_data(cmd))

    def recv_data(self):
        #TODO implement protocol
        out = self.session.recv(1024)
        return self.unpack_data(out)

    # packs data into json
    def pack_data(self, data):
        #TODO pack data
        return data

    # unpacks json data
    def unpack_data(self, data):
        #TODO unpack data
        return data

                    

c = Client()
c.start()

        
