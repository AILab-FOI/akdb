# instructions
# you start the client with
#	python akdb_cli.py
# and start sending the commands
# you have an in-app help that you can raise up with \?, 
# and you can easily disconnect and exit with \q.

import cmd
import socket
import colors

welcome_note =  '''Welcome to the AKDB client. Enter your commands in the command line.
To see the list of all commands type '\h' or 'help'

To see list of all commands, type '\?'
To see list of entered comands, type 'history'.
'''

client_specific_commands = ["", "\?", "history"]
server_commands = []

command_list =  {'\q': 'Disconnect from the server and quit akdb client.',
				 '\?': 'List all commands supported in akdb.',
				 'history': 'List all previously typed commands.'
				}

#instantiate the colours class
bcolors = colors.bcolors()

#define data
ip = "localhost"
port = 1999
buffer_size = 1024


class AK_Console(cmd.Cmd):
	cli = None

	#initialization of the console
	def __init__(self):
		cmd.Cmd.__init__(self)
		self.prompt = "akdb>"
		self.intro = welcome_note

	#setter for the AK_client class
	def set_cli(self, cli):
		self.cli = cli

	#when empty line is pressed, call 
	#default and ignore it
	def emptyline(self):
		self.default("")

	# called when history command is called
	def do_history(self, args):
		print self._history

	#called before loop is started
	#defines history list and local and 
	#global variables that are currently not used
	def preloop(self):
		cmd.Cmd.preloop(self)
		self._history = []
		self._locals = {}
		self._globals = {}

	#called before the command is executed
	def precmd(self, line):
		self._history += [line.strip()]
		return line

	#called after the command is eqecuted
	def postcmd(self, stop, line):
		if line in client_specific_commands:
			return
		response = self.cli.AK_check_response()
		if response == "\q":
			self.cli.set_finished(True)
			return -1
		else:
			print response

	#called on default
	#when the command is inserted
	def default(self, line):
		if line == "":
			return
		elif line == "\?":
			self.print_commands()
			return
		line = line.strip()
		self.cli.AK_send_command(line)

	#prints the commands when the \? is inserted
	def print_commands(self):
		print bcolors.YELLOW + "Commands:"
		for k, v in command_list.iteritems():
			print bcolors.OKBLUE +  k + " - " + bcolors.OKGREEN + v + bcolors.ENDC
		print ""
		return

#client class
class AK_client():
	sock = None
	finished = False

	# connects to the server
	#runs a loop that holds the commands
	def AK_client_connect(self, ip, port, buffer_size):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect((ip, port))
		self.AK_check_response()
		c =  AK_Console()
		c.set_cli(self)
		while self.finished != True:
			c.cmdloop()
		print "\nAKDB now exiting."

	#sends a command to server
	def AK_send_command(self, command):
		self.sock.send(command.strip())

	#checks for a command from the server
	def AK_check_response(self):
		return self.sock.recv(buffer_size)

	#setter for finished vairable
	def set_finished(self, val):
		self.finished = val

	#getter for the finished variable
	def get_finished(self):
		return self.finished

cli = AK_client()

if __name__ == "__main__":
	cli.AK_client_connect(ip, port, buffer_size)

