#!/usr/bin/env python2

import sys
import time
import paramiko

if len(sys.argv) == 3:
    username = sys.argv[1]
    password = sys.argv[2]
else:
    username = raw_input("Username: ")
    password = raw_input("Password: ")


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
        if self.session is not None:
            self.session.close()
            print "[+] Session closed succesfully"
        self.sock.close()
        print "[+] Socket closed successfully"

    def start(self):
        global username
        global password
        connected = False
        
        while not connected:
            print "[*] Attempting to connect..."
            print username,password
            try:
                self.sock.connect(
                    hostname=self.host, 
                    port=self.port, 
                    username=username, 
                    password=password
                )
                connected = True
            except paramiko.ssh_exception.AuthenticationException:
                username = raw_input("Username: ")
                password = raw_input("Password: ")
            except Exception, e:
                # wait 2 seconds before attempting to connect again
                time.sleep(2)

        self.session = self.sock.get_transport().open_session()
        print "[+] Successfully conected to server"

        while self.working:
            try:
                cmd = raw_input("akdb> ")
                self.send_command(cmd)
                out = self.recv_data()
                print out
            except KeyboardInterrupt:
                self.working = False


    def send_command(self, cmd):
        #TODO implement protocol
        if len(cmd) > 0:
            try:
                self.session.send(self.pack_data(cmd))
            except Exception, e:
                print "[-] Sending failed: %s" %e

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

        
