# instructions
# you start the server with:
#   python server.py
# and it runs indefinitely
# currently it needs to be turned off manually ctrl+c

import sys
sys.path.append('../swig/')
import SocketServer
import time
import kalashnikovDB as ak47
import sql_executor as sqle

class AK_server(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass

class AK_server_request_handler(SocketServer.BaseRequestHandler):
    '''
    Handles requests
    '''
    executor = sqle.sql_executor()

    def setup(self):
        '''
        Called when client connects, before handle
        '''
        print self.client_address, "connected"
        self.request.send("Connection successful")

    def handle(self):
        """
        Handles requests by calling the processCommand method.
        """
        while True:
            response = self.processCommand(self.request.recv(1024).strip())
            if response == "\q":
                self.request.send(response)
                return
            else:
                self.request.send(response)

    def finish(self):
        """
        Called when client disconnects, after handle.
        """
        print self.client_address, "disconnected"

    #TODO command processing goes here
    def processCommand(self, command):
        """
        Used to process a command given from a client.
        Here is where you should call your application logic code.
        """
        if command=="\q":
            return command
        else:
            return self.executor.execute(command)

if __name__ == "__main__":
    ip = "localhost"
    port = 1998
    server = AK_server((ip, port), AK_server_request_handler)
    server.serve_forever()