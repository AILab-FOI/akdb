# instructions
# you start the server with:
#   python server.py 
# and it runs indefinitely
# currently it needs to be turned off manually ctrl+c


import SocketServer
import time

class AK_server(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass


class AK_server_request_handler(SocketServer.BaseRequestHandler):
    '''
    Handles requests
    '''

    def setup(self):
        '''
        Called when client connects, before handle
        '''
        print self.client_address, "connected"
        self.request.send("Connection successful")

    def handle(self):
        '''
        Handles requests
        '''
        while True:
            response = self.processCommand(self.request.recv(1024).strip())
            if response == "\q":
                self.request.send(response)
                return
            else:
                self.request.send(response)

    def finish(self):
        '''
        Called when client disconnects, after handle
        '''
        print self.client_address, "disconnected"

    #TODO command processing goes here
    def processCommand(self, command):
        if command=="\q":
            return command
        return command + " OK"

if __name__ == "__main__":
    ip = "localhost"
    port = 1999
    server = AK_server((ip, port), AK_server_request_handler)
    server.serve_forever()