#file server_test.py runs the server for testing

from server import *

ip="localhost"
port=1999
server=AK_server((ip, port), AK_server_request_handler)
server.serve_forever()
