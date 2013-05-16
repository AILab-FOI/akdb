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

To see list of all commands, type '\?'
To see list of entered comands, type 'history'.
'''

#List of commands that don't send request to server
#You should add commands to this list when needed
client_specific_commands = ["", "\?", "history"]

#command list, used to print out the help
#update this list when new commands are added
command_list =  {'\q': 'Disconnect from the server and quit akdb client.',
				 '\?': 'List all commands supported in akdb.',
				 'history': 'List all previously typed commands.'
				}

#instantiate the colours class
bcolors = colors.bcolors()

#define data
ip = "localhost"
port = 1998
buffer_size = 1024


class AK_Console(cmd.Cmd):
	cli = None

	def __init__(self):
		"""
		Initialization of the console. Sets up the prompt and
		a welcome note.
		"""
		cmd.Cmd.__init__(self)
		self.prompt = "akdb>"
		self.intro = welcome_note

	def set_cli(self, cli):
		"""
		Setter for the AK_client class
		"""
		self.cli = cli

	def emptyline(self):
		"""
		When empty line is pressed, call default and ignore it
		"""
		self.default("")

	def do_history(self, args):
		"""
		Called when history command is called
		"""
		print self._history

	def preloop(self):
		"""
		Called before loop is started defines history list and local and
		global variables that are currently not used
		"""
		cmd.Cmd.preloop(self)
		self._history = []
		self._locals = {}
		self._globals = {}

	def precmd(self, line):
		"""
		Called before the command is executed
		"""
		self._history += [line.strip()]
		return line

	def postcmd(self, stop, line):
		"""
		Called after the command is eqecuted
		"""
		if line in client_specific_commands:
			return
		response = self.cli.AK_check_response()
		if response == "\q":
			self.cli.set_finished(True)
			return -1
		else:
			print response

	def default(self, line):
		"""
		Called on default when the command is inserted
		"""
		if line == "":
			return
		elif line == "\?":
			self.print_commands()
			return
		line = line.strip()
		self.cli.AK_send_command(line)

	def print_commands(self):
		"""
		Prints the commands when the \? is inserted
		"""
		print bcolors.YELLOW + "Commands:"
		for k, v in command_list.iteritems():
			print bcolors.OKBLUE +  k + " - " + bcolors.OKGREEN + v + bcolors.ENDC
		print ""
		return

#client class
class AK_client():
	sock = None
	finished = False

	def AK_client_connect(self, ip, port, buffer_size):
		"""
		Connects to the server. runs a loop that holds the commands
		"""
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect((ip, port))
		self.AK_check_response()
		c =  AK_Console()
		c.set_cli(self)
		while self.finished != True:
			c.cmdloop()
		print "\nAKDB now exiting."

	def AK_send_command(self, command):
		"""
		Sends a command to server
		"""
		self.sock.send(command.strip())

	def AK_check_response(self):
		"""
		Checks for a command from the server
		"""
		return self.sock.recv(buffer_size)

	def set_finished(self, val):
		"""
		Setter for finished vairable
		"""
		self.finished = val

	def get_finished(self):
		"""
		Getter for the finished variable
		"""
		return self.finished

cli = AK_client()

if __name__ == "__main__":
	cli.AK_client_connect(ip, port, buffer_size)

