#!/usr/bin/env python2

import paramiko

HOST = "localhost"
PORT = 1998

username = "testingUser" # raw_input("Username: ")
password = "testingPass" # raw_imput("Password: ")

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
