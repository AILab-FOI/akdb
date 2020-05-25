#!/usr/bin/env python2
import sys
from client import Client

sys.path.append("../swig/")
import kalashnikovDB as ak47

#TODO refactor code ( split logic to multiple files)
if len(sys.argv) == 3:
    username = sys.argv[1]
    password = sys.argv[2]
else:
    username = raw_input("Username: ")
    password = raw_input("Password: ")

c = Client(username=username, password=password)
c.start()
