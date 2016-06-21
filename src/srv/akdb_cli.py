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

##List of commands that don't send request to server
#You should add commands to this list when needed
client_specific_commands = ["", "\?", "history"]

##command list, used to print out the help
#update this list when new commands are added
command_list =  {'\q': 'Disconnect from the server and quit akdb client.',
				 '\?': 'List all commands supported in akdb.',
				 '\d <table_name>' : 'Prints out table details',
				 '\p <table_name>' : 'Prints out table',
				 '\\t <table_name>?': 'Check whether the given table exists in database or not.',
				 'create sequence <sequence_name> <sql_expression>' : 'Creating sequence in AK_sequence table',
				 'create table <table_name> <sql_expression>' : 'Create table',
				 'create view <view_name> as <sql_expression' : 'Create view',
				 'create index <index_name> <sql_expression>' : 'Create index',
				 'create trigger <trigger_name> <sql_expression>' : 'Create trigger',
				 'create user <username> with password <password' : 'Create user',
				 'create function <function_name> <args> returns <sql_expression> <language>' : 'Create function',
				 'drop <sql_expression>' : 'Drop sql object',
				 'grant <privilege_name> <sql_expression>' : 'Grant privilege',
				 'select <sql_expression>' : 'Select data from table',
				 'update <sql_expression>' : 'Update table',
				 'insert into <table_name> <sql_expression>' : 'Insert command',
				 'alter table <table_name> <sql_expression>' : 'Alter table',
				 'alter index <index_name> <command>' : 'Alter index',
				 'alter sequence <sequence_name> <command>' : 'Alter sequence',
				 'alter view <view_name> <command>' : 'Alter view',
				 'begin <sql_expression> commit' : 'Transaction',
				 'history': 'List all previously typed commands.'
				}

##instantiate the colours class
bcolors = colors.bcolors()

##define data
ip = "localhost"
port = 1998
buffer_size = 1024


class AK_Console(cmd.Cmd):
	cli = None

	##Initialization of the console. Sets up the prompt and
	#a welcome note.
	def __init__(self):
		cmd.Cmd.__init__(self)
		self.prompt = "akdb>"
		self.intro = welcome_note

	##Setter for the AK_client class
	def set_cli(self, cli):
		self.cli = cli

	##When empty line is pressed, call default and ignore it
	def emptyline(self):
		self.default("")

	##Called when history command is called
	def do_history(self, args):
		print self._history

	##Called before loop is started defines history list and local and
	#global variables that are currently not used
	def preloop(self):
		cmd.Cmd.preloop(self)
		self._history = []
		self._locals = {}
		self._globals = {}

	##Called before the command is executed
	def precmd(self, line):
		self._history += [line.strip()]
		return line

	##Called after the command is eqecuted
	def postcmd(self, stop, line):
		if line in client_specific_commands:
			return
		response = self.cli.AK_check_response()
		if response == "\q":
			self.cli.set_finished(True)
			return -1
		else:
			print response

	##Called on default when the command is inserted
	def default(self, line):
		if line == "":
			return
		elif line == "\?":
			self.print_commands()
			return
		line = line.strip()
		self.cli.AK_send_command(line)

	##Prints the commands when the \? is inserted
	def print_commands(self):
		print bcolors.YELLOW + "Commands:"
		for k, v in command_list.iteritems():
			print bcolors.OKBLUE +  k + " - " + bcolors.OKGREEN + v + bcolors.ENDC
		print ""
		return

##client class
class AK_client():
	sock = None
	finished = False

	##Connects to the server. runs a loop that holds the commands
	def AK_client_connect(self, ip, port, buffer_size):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect((ip, port))
		self.AK_check_response()
		c =  AK_Console()
		c.set_cli(self)
		while self.finished != True:
			c.cmdloop()
		print "\nAKDB now exiting."

	##Sends a command to server
	def AK_send_command(self, command):
		self.sock.send(command.strip())

	##Checks for a command from the server
	def AK_check_response(self):
		return self.sock.recv(buffer_size)
	##Setter for finished vairable
	def set_finished(self, val):
		self.finished = val

	##Getter for the finished variable
	def get_finished(self):
		return self.finished

cli = AK_client()

if __name__ == "__main__":
	cli.AK_client_connect(ip, port, buffer_size)

