#!/usr/bin/env python2

import paramiko

HOST = "localhost"
PORT = 1998

ssh_client = paramiko.SSHClient()
ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

ssh_client.connect(HOST, port=PORT, username="testingUser", password="testingPass")
session = ssh_client.get_transport().open_session()

print session.recv(1024)
cmd = raw_input("akdb> ")
session.send(cmd)
print session.recv(1024)

session.close()
ssh_client.close()
