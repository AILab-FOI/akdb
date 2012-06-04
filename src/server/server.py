import SocketServer


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
            response=self.processCommand(self.request.recv(1024))
            if response=="disconnect":
                return
            self.request.send(response)
            
    def finish(self):
        '''
        Called when client disconnects, after handle
        '''      
        print self.client_address, "disconnected"

    #TODO command processing goes here
    def processCommand(self, command):
        if command=="disconnect":
            return command
        return command+" OK"
