# instructions
# you start the server with:
#   python server.py
# and it runs indefinitely
# currently it needs to be turned off manually ctrl+c

import sys
sys.path.append('../swig/')
import SocketServer
import socket
import time
import kalashnikovDB as ak47
import sql_executor as sqle
import colors
from user_log import logging

logging.info('Server successfully started')
##instantiate the colours class
bcolors = colors.bcolors()

class AK_server(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass

## request_handler_class
# Handles requests
class AK_server_request_handler(SocketServer.BaseRequestHandler):
    executor = sqle.sql_executor()

    ## setup method
    # Called when client connects, before handle
    def setup(self):
        print self.client_address, "connected"
        self.request.send("Connection successful")

    ## handle method
    # Handles requests by calling the processCommand method.
    def handle(self):
        try:
            while True:
                response = self.processCommand(self.request.recv(1024).strip())
                if response == "\q":
                    self.request.send(response)
                    return
                else:
                    self.request.send(response)
        except socket.error, e:
            print bcolors.FAIL + "An error occured. Probably the client disconnected violently." + bcolors.ENDC

    ## finish method
    #Called when client disconnects, after handle.
    def finish(self):
        print self.client_address, "disconnected"
    ## process command method
    # Used to process a command given from a client.
    # Here is where you should call your application logic code.
    def processCommand(self, command):
	logging.info(command)
        if command=="\q":
            return command
        else:
            return self.executor.execute(command)

if __name__ == "__main__":
    ip = "localhost"
    port = 1998
    sqle.initialize()
    print "\nServer is up and running!\nPort number: 1998\n"
    server = AK_server((ip, port), AK_server_request_handler)
    server.serve_forever()
