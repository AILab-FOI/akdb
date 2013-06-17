#!/usr/bin/env python

from sql_tokenizer import *
import time

import sys
sys.path.append('../swig/')
import kalashnikovDB as ak47
import test
import re



def initialize():
        ak47.AK_inflate_config()
        ak47.AK_init_disk_manager()
        ak47.AK_memoman_init()

## is_numeric
# returns int or float value based on input type
# @param lit the value to be checked
def is_numeric(lit):
    # Handle '0'
    if lit == '0': return 0
    # Hex/Binary
    litneg = lit[1:] if lit[0] == '-' else lit
    if litneg[0] == '0':
        if litneg[1] in 'xX':
            return int(lit,16)
        elif litneg[1] in 'bB':
            return int(lit,2)
        else:
            try:
                return int(lit,8)
            except ValueError:
                pass
    # Int/Float
    try:
        return int(lit)
    except ValueError:
        pass
    try:
        return float(lit)
    except ValueError:
        pass

## is_date
# returns true if the input value is in date format
# @param lit the value to be checked
def is_date(lit):
        try:
          time.strptime(lit, '%Y-%m-%d')
          return True
        except ValueError:
          return False

## is_datetime
# returns true if the input value is in datetime format
# @param lit the value to be checked
def is_datetime(lit):
        returnVal = 2
        try:
                time.strptime(lit, '%Y-%m-%d %H:%M:%S')
                returnVal += 1
        except ValueError:
                returnVal -= 1
        try:
                time.strptime(lit, '%Y-%m-%d %H:%M:%S.%f')
                returnVal += 1
        except ValueError:
                returnVal -= 1
        if returnVal > 0:
                return True
        else:
                return False

## is_time
# returns true if the input value is in time format
# @param lit the value to be checked
def is_time(lit):
        try:
                time.strptime(lit, '%H:%M:%S')
                return True
        except ValueError:
                return False

## is_bool
# returns true if the input value is boolean
# @param lit the value to be checked
def is_bool(lit):
        return lit.lower() in ("true","false")

## get_attr_type
# returns value type code in ak47 notation defined in constants.c
# @param value the value to be checked
def get_attr_type(value):

        integer = type(is_numeric(value)) == int
        decimal = type(is_numeric(value)) == float
        varchar = isinstance(value,str)
        date = is_date(value.replace("'",""))
        datetime = is_datetime(value.replace("'",""))
        time = is_time(value.replace("'",""))
        boolean = is_bool(value.replace("'",""))

        if integer:
                return ak47.TYPE_INT
        elif decimal:
                return ak47.TYPE_FLOAT
        elif date:
                return ak47.TYPE_DATE
        elif datetime:
                return ak47.TYPE_DATETIME
        elif time:
                return ak47.TYPE_TIME
        elif boolean:
                return ak47.TYPE_BOOL
        elif varchar:
                return ak47.TYPE_VARCHAR
        else:
                print "UNDEFINED"

## get_type_name
# returns type name for supplied type code in ak47 notation defined in constants.c
# @param code the type code to be checked
def get_type_name(code):

        if code == ak47.TYPE_INT:
                return "int"
        elif code == ak47.TYPE_FLOAT:
                return "float"
        elif code == ak47.TYPE_DATE:
                return "date"
        elif code == ak47.TYPE_DATETIME:
                return "datetime"
        elif code == ak47.TYPE_TIME:
                return "time"
        elif code == ak47.TYPE_BOOL:
                return "boolean"
        elif code == ak47.TYPE_VARCHAR:
                return "varchar"
        else:
                return "UNDEFINED"

## akdbError
# prints expression expr and a pointer at the position of error item
# @param expr the expression to be printed
# @param item item where the error occured
def akdbError(expr,item):
        print expr
        print " " * expr.index(item) + "^"

## print_table_command
# defines the structure of print command and its execution
class Print_table_command:

        print_regex = r"^\\p\s+([a-zA-Z0-9_]+)\s*$"
        pattern = None
        matcher = None

        ## matches method
        # checks whether given input matches print command syntax
        def matches(self, input):
                self.pattern = re.compile(self.print_regex)
                self.matcher = self.pattern.match(input)
                return self.matcher != None

        ## execute method
        # defines what is called when print command is invoked
        def execute(self):
                ak47.AK_print_table(self.matcher.group(1))
                return "OK"

## table_derails_command
# defines the structure of table details command and its execution
class Table_details_command:

        table_details_regex = r"^\\d\s+([a-zA-Z0-9_]+)\s*$"
        pattern = None
        matcher = None

        ## matches method
        # checks whether given input matches table_details command syntax
        def matches(self, input):
                self.pattern = re.compile(self.table_details_regex)
                self.matcher = self.pattern.match(input)
                return self.matcher != None

        ## execute method
        # defines what is called when table_details command is invoked
        def execute(self):
                print "Printing out: "
                result = "Number of attributes: " + str(ak47.AK_num_attr(self.matcher.group(1)))
                result += "\nNumber od records: " + str(ak47.AK_get_num_records(self.matcher.group(1)))
                return result

class Table_exists_command:

        table_details_regex = r"^\\t\s+([a-zA-Z0-9_]+)\?\s*$"
        pattern = None
        matcher = None

        ## matches method
        # checks whether given input matches table_exists command syntax
        def matches(self, input):
                self.pattern = re.compile(self.table_details_regex)
                self.matcher = self.pattern.match(input)
                return self.matcher != None

        ## execute method
        # defines what is called when table_exists command is invoked
        def execute(self):
                if ak47.AK_table_exist(self.matcher.group(1)) == 0:
                        result = "Table does not exist."
                else:
                        result = "Table exists. You can see it by typing \p <table_name>."
                return result

## create sequence
# developd by Danko Sacer
class Create_sequence_command:  
        create_seq_regex = r"^(?i)create sequence(\s([a-zA-Z0-9_]+))+?$"
        pattern = None
        matcher = None

        ## matches method
        # checks whether given input matches table_exists command syntax
        def matches(self, input):
                print "matching regex"
                self.pattern = re.compile(self.create_seq_regex)
                self.matcher = self.pattern.match(input)
                if (self.matcher is not None):
                        message = self.matcher
                else:
                        message = None
                
                return message 

        
        # executes the create sequence expression
        # neded revision in sequence.c in function AK_sequence_add which receives 
        # only int values but posible is also bigint which is default for undefined values
        def execute(self):
                print "start parsing.."
                print self.matcher.group(0)
                print "after self"
                pars = sql_tokenizer()
                tok = pars.AK_create_sequence(self.matcher.group(0))
                # isinstance needs revision for swig
                '''
                if isinstance(tok, str):
                        print "Error: syntax error in expression"
                        print string
                        print tok
                        return False
                '''
                print "\nSequence name: ", tok.seq_name
                print "'AS' definition: ", tok.as_value
                print "'Start with' value: ", tok.start_with
                print "'Increment by' value: ", tok.increment_by
                print "'MinValue' value: ", tok.min_value
                print "'MaxValue' value: ", tok.max_value
                print "'Cache' value: ", tok.cache
                print "'Cycle' value: ", tok.cycle
                
                # Check for sequence name, if already exists in database return false
                # Needs more revision for swig after buffer overflow is handled
                '''
                names = ak47.AK_get_column(1, "AK_sequence")
                for name in set(names):
                        if(name==tok.seq_name):
                                error = "ERROR the name is already used"
                                return error
                '''
                # executing create statement 
                try:
                        ak47.AK_sequence_add(str(tok.seq_name), int(tok.start_with), int(tok.increment_by), int(tok.max_value), int(tok.min_value), int(tok.cycle))
                        result = "Command succesfully executed"
                except:
                        result = "ERROR creating sequence didn't pass"
                
                ak47.AK_print_table("AK_sequence")
                return result
                
## create table command
# @author Franjo Kovacic
class Create_table_command:
	create_table_regex = r"^(?i)create table(\s([a-zA-Z0-9_]+))+?$"
        pattern = None
        matcher = None

        ## matches method
        # checks whether given input matches table_exists command syntax
        def matches(self, input):
                print "matching regex"
                self.pattern = re.compile(self.create_table_regex)
                self.matcher = self.pattern.match(input)
                if (self.matcher is not None):
                        message = self.matcher
                else:
                        message = None
                
                return message 

        
        # executes the create table expression
        def execute(self):
                print "start parsing.."
                print self.matcher.group(0)
                parser = sql_tokenizer()
                token = parser.AK_parse_create_table(expr)
                # checking syntax
                if isinstance(token, str):
                        print "Error: syntax error in expression"
                        print expr
                        print token
                        return False
                # get table name
                table_name = str(token.tableName)
                # table should not exist yet
                '''
                For some reason, AK_table_exist won't work, it always just exits here, so it's commented out
                if (ak47.AK_table_exist(table_name) == 1):
                        print "Error: table'" + table_name + "' already exist"
                        return False
                '''                
                # get attributes
                '''
                Create table in table.c currently takes only name and type of attributes.
                Parsing works for other attribute properties as well, so it should be added here when possible.
                '''
                create_table_attributes = []
                for attribute in token.attributes:
                        create_table_attributes.append([{'name': str(attribute[0])}, {'type':get_attr_type(str(attribute[1]))}])
                attribute_count = len(create_table_attributes)
                # executing
                try:
                        ak47.AK_create_table(table_name, create_table_attributes, attribute_count)
                        result = "Table created"
                except:
                        result = "Error. Creating table failed."
                return result

## create index command
# @author Franjo Kovacic
class Create_index_command:
	create_table_regex = r"^(?i)create index(\s([a-zA-Z0-9_]+))+?$"
        pattern = None
        matcher = None

        ## matches method
        # checks whether given input matches table_exists command syntax
        def matches(self, input):
                print "matching regex"
                self.pattern = re.compile(self.create_index_regex)
                self.matcher = self.pattern.match(input)
                if (self.matcher is not None):
                        message = self.matcher
                else:
                        message = None
                
                return message 

        
        # executes the create index expression
        def execute(self):
                print "start parsing.."
                print self.matcher.group(0)
                parser = sql_tokenizer()
                token = parser.AK_parse_createIndex(expr)
                # checking syntax
                if isinstance(token, str):
                        print "Error: syntax error in expression"
                        print expr
                        print token
                        return False
                #get table name
                table_name = str(token.tablica)
                # check if table exist
                '''
                For some reason, AK_table_exist won't work, it always just exits here, so it's commented out
                if (ak47.AK_table_exist(table_name) == 0):
                        print "Error: table '"+ table_name +"' does not exist"
                        return False
                '''
                #get index name
                index = str(token.IndexIme)
                #get other expression tokens
                t = list()
                t.append(table_name)
                t.append(token.stupci)
                t.append(token.IndexVrsta)
                #executing
                '''
                Not working
                TypeError: in method 'AK_create_Index', argument 2 of type 'AK_list *'
                '''
                try:
                        ak47.AK_create_Index(index, t)
                        result = "Index created"
                except:
                        result = "Error. Creating index failed."
                return result

## create trigger command
# @author Franjo Kovacic
class Create_trigger_command:
	create_trigger_regex = r"^(?i)create trigger(\s([a-zA-Z0-9_]+))+?$"
        pattern = None
        matcher = None

        ## matches method
        # checks whether given input matches table_exists command syntax
        def matches(self, input):
                print "matching regex"
                self.pattern = re.compile(self.create_trigger_regex)
                self.matcher = self.pattern.match(input)
                if (self.matcher is not None):
                        message = self.matcher
                else:
                        message = None
                
                return message 

        
        # executes the create trigger expression
        def execute(self):
                print "start parsing.."
                print self.matcher.group(0)
                parser = sql_tokenizer()
                token = parser.AK_parse_trigger(expr)
                # checking syntax
                if isinstance(token, str):
                        print "Error: syntax error in expression"
                        print expr
                        print token
                        return False
                #get table name
                table_name = str(token.tableName)
                # check if table exist
                '''
                For some reason, AK_table_exist won't work, it always just exits here, so it's commented out
                if (ak47.AK_table_exist(table_name) == 0):
                        print "Error: table '"+ table_name +"' does not exist"
                        return False
                '''
                #get trigger name
                trigger = str(token.name)
                p = list()
                p.append(token.EventOption1)
                p.append(token.EventOption2)
                p.append(token.EventOption3)
                #executing
                '''
                Not working
                TypeError: in method 'AK_trigger_add', argument 3 of type 'AK_list *'
                '''
                try:
                        ak47.AK_trigger_add(trigger, token.whatOption, p, table_name, token.functionName)
                        result = "Trigger created"
                except:
                        result = "Error. Creating trigger failed."
                return result
                
## Insert into
class Insert_into_command:

    insert_into_regex = r"^(?i)insert into(\s([a-zA-Z0-9_\(\),'\.]+))+?$"
    pattern = None
    matcher = None

    def matches(self,inp):
        self.pattern = re.compile(self.insert_into_regex)
        self.matcher = self.pattern.match(inp)
        return self.matcher != None


    def execute(self):
        expr = self.matcher.group(0)
        parser = sql_tokenizer()
        token = parser.AK_parse_insert_into(expr)
        if isinstance(token, str):
                print "Error: syntax error in expression"
                print expr
                print token
                return False
        table_name = str(token.tableName)
        # postoji li tablica
        if (ak47.AK_table_exist(table_name) == 0):
                print "Error: table '"+ table_name +"' does not exist"
                return False
        # vrijednosti podataka za unos
        insert_attr_values = map(lambda x: x.replace("'",""),list(token.columnValues[0]))
        # tipovi podataka za unos
        insert_attr_types = map(lambda x: get_attr_type(x.replace("'","")),list(token.columnValues[0]))
        #Dohvatiti listu atributa tablice
        table_attr_names = str(ak47.AK_rel_eq_get_atrributes_char(table_name)).split(";")
        #Dohvatiti tipove atributa tablice
        table_attr_types = str(ak47.AK_get_table_atribute_types(table_name)).split(";")
        # imena atributa za unos
        insert_attr_names = table_attr_names
        # navedeni su atributi za unos
        if(token.columns):
                insert_attr_names = []
                table_types_temp = table_attr_types
                table_attr_types = []
                insert_columns = list(token.columns[0])
                for index,col in enumerate(insert_columns):
                        if col not in table_attr_names:
                                print "\nError: table has no attribute '" + str(col) + "':"
                                akdbError(expr,col)
                                return False
                #provjera atributa za unos
                for ic,col in enumerate(insert_columns):
                        for ia,tab in enumerate(table_attr_names):
                                if col == tab:
                                        if tab not in insert_attr_names:
                                                insert_attr_names.append(tab)
                                                table_attr_types.append(int(table_types_temp[ia]))
                                        else:
                                                print "\nError: duplicate attribute " + tab + ":"
                                                akdbError(expr,tab)
                                                return False

                if (len(insert_columns) == len(insert_attr_values)):
                        for index,tip in enumerate(insert_attr_types):
                                if int(insert_attr_types[index]) != int(table_attr_types[index]):
                                        type_name = get_type_name(int(table_attr_types[index]))
                                        print "\nError: type error for attribute '" + insert_attr_names[index] + "':"
                                        akdbError(expr,insert_attr_values[index])
                                        print "Expected: " + type_name
                                        return False
                else:
                        print "\nError: attribute names number not matching attribute values number supplied for table '" + table_name + "':"
                        akdbError(expr,insert_columns[0])
                        return False
        # navedene su samo vrijednosti za unos
        elif (len(table_attr_names) < len(insert_attr_values)):
                print "\nError: too many attibutes, table " + str(token.tableName) + " has " + str(len(table_attr_names)) 
                return False
        elif (len(table_attr_names) > len(insert_attr_values)):
                print "\nError: too few attibutes, table " + str(token.tableName) + " has " + str(len(table_attr_names))
                return False
        else:
                for index,tip in enumerate(insert_attr_types):
                        if insert_attr_types[index] != int(table_attr_types[index]):
                                type_name = get_type_name(int(table_attr_types[index]))
                                print "\nError: type error for attribute '" + insert_attr_names[index] + "':"
                                akdbError(expr,insert_attr_values[index])
                                print "Expected: " + type_name
                                return False
        if(ak47.insert_data_test(table_name,insert_attr_names,insert_attr_values,insert_attr_types) == ak47.EXIT_SUCCESS):
                return True
        else:
                return False
        return False


## create group command
# @author Zvonimir Kapes
class Create_group_command:    
    
    table_details_regex = r"^create group\s+"
    pattern = None
    matcher = None
    command = None
 
    def matches(self, input):
        self.pattern = re.compile(self.table_details_regex, re.IGNORECASE)
        self.matcher = self.pattern.match(input)
        self.command = input
        return self.matcher != None

    def execute(self):
         print "Printing out: "
         result = "CREATE GROUP "
         
         pars = sql_tokenizer()
         tokens = pars.AK_create_group(self.command)
         
         
         if isinstance(tokens, basestring):
             result = "Wrong command!"
         else:
             print "group id: ", ak47.AK_group_get_id(tokens.groupname)
             
             if ak47.AK_group_get_id(tokens.groupname) != -1:
                 result += "ERROR: Group " + str(tokens.groupname) +" already exists"
                 return result
                 
             result += str(ak47.AK_group_add(tokens.groupname, ak47.NEW_ID))
             
             if tokens.users:
                 for user in tokens.users:
                     print "...added user: ", user
                     # User insert does not work. Password needs to be inserted in hash format.
                     #result += str(ak47.AK_user_add(user, password, ak47.NEW_ID))
                     #result += str(ak47.AK_add_user_to_group(user, tokens.groupname))
                 
         return result

## grant command
# @author Zvonimir Kapes
class Grant_command:    
    
    table_details_regex = r"^grant\s+"
    pattern = None
    matcher = None
    command = None
 
    def matches(self, input):
        self.pattern = re.compile(self.table_details_regex, re.IGNORECASE)
        self.matcher = self.pattern.match(input)
        self.command = input
        return self.matcher != None

    def execute(self):
         print "Printing out: "
         result = "GRANT "
         
         pars = sql_tokenizer()
         tokens = pars.AK_parse_grant(self.command)
         
         
         if isinstance(tokens, basestring):
             result = "Wrong command!"
         else:
             for user in tokens.users:
                 msg = "...grant to: " + user + "\n"
                 for privilege in tokens.privileges:
                     msg += "...granted privilege: " + privilege + "\n"
                     for table in tokens.tables:
                         msg += "...on table: " + table + "\n"
                         if tokens.group:
                             res = ak47.AK_grant_privilege_group(user, table, privilege)
                             result += str(res)
                             
                             if res != -1:
                                 print msg
                                 msg = ""
                             else:
                                 print "ERROR: Group or table does not exsist: " + str(user) + str(table)
                         else:
                             res = ak47.AK_grant_privilege_user(user, table, privilege)
                             result += str(res)
                             
                             if res != -1:
                                 print msg
                                 msg = ""
                             else:
                                 print "ERROR: User or table does not exsist: " + str(user) + str(table)
                                 
                 
         return result


## sql_executor
# contaions methods for sql operations
class sql_executor:

        ##initialize classes for the available commands
        print_command =  Print_table_command()
        table_details_command = Table_details_command()
        table_exists_command = Table_exists_command()
        create_sequence_command = Create_sequence_command()
        create_table_command = Create_table_command()
        create_index_command = Create_index_command()
        create_trigger_command = Create_trigger_command()
        insert_into_command = Insert_into_command()
	create_group_command = Create_group_command()
	grant_command = Grant_command()

        ##add command instances to the commands array
        commands = [print_command, table_details_command, table_exists_command, create_sequence_command, create_table_command, create_index_command, create_trigger_command,insert_into_command, create_group_command, grant_command]

        ## commands for input
        # checks whether received command matches any of the defined commands for kalashnikovdb, 
        # and call its execution if it matches
        def commands_for_input(self, command):
                for elem in self.commands:
                        if (elem.matches(command) is not None):
                                return elem.execute()
                return "Wrong command."

        ## execute method
        # called when a new command is received (from client)
        def execute(self, command):
                return self.commands_for_input(command)

        ## insert
        # executes the insert expression
        # @param self object pointer
        # @parama expr the expression to be executed 
        def insert(self,expr):
                parser = sql_tokenizer()
                token = parser.AK_parse_insert_into(expr)
                if isinstance(token, str):
                        print "Error: syntax error in expression"
                        print expr
                        print token
                        return False
                table_name = str(token.tableName)
                # postoji li tablica
                if (ak47.AK_table_exist(table_name) == 0):
                        print "Error: table '"+ table_name +"' does not exist"
                        return False
                # vrijednosti podataka za unos
                insert_attr_values = map(lambda x: x.replace("'",""),list(token.columnValues[0]))
                # tipovi podataka za unos
                insert_attr_types = map(lambda x: get_attr_type(x.replace("'","")),list(token.columnValues[0]))
                #Dohvatiti listu atributa tablice
                table_attr_names = str(ak47.AK_rel_eq_get_atrributes_char(table_name)).split(";")
                #Dohvatiti tipove atributa tablice
                table_attr_types = str(ak47.AK_get_table_atribute_types(table_name)).split(";")
                # imena atributa za unos
                insert_attr_names = table_attr_names
                # navedeni su atributi za unos
                if(token.columns):
                        insert_attr_names = []
                        table_types_temp = table_attr_types
                        table_attr_types = []
                        insert_columns = list(token.columns[0])
                        for index,col in enumerate(insert_columns):
                                if col not in table_attr_names:
                                        print "\nError: table has no attribute '" + str(col) + "':"
                                        akdbError(expr,col)
                                        return False
                        #provjera atributa za unos
                        for ic,col in enumerate(insert_columns):
                                for ia,tab in enumerate(table_attr_names):
                                        if col == tab:
                                                if tab not in insert_attr_names:
                                                        insert_attr_names.append(tab)
                                                        table_attr_types.append(int(table_types_temp[ia]))
                                                else:
                                                        print "\nError: duplicate attribute " + tab + ":"
                                                        akdbError(expr,tab)
                                                        return False

                        if (len(insert_columns) == len(insert_attr_values)):
                                for index,tip in enumerate(insert_attr_types):
                                        if int(insert_attr_types[index]) != int(table_attr_types[index]):
                                                type_name = get_type_name(int(table_attr_types[index]))
                                                print "\nError: type error for attribute '" + insert_attr_names[index] + "':"
                                                akdbError(expr,insert_attr_values[index])
                                                print "Expected: " + type_name
                                                return False
                        else:
                                print "\nError: attribute names number not matching attribute values number supplied for table '" + table_name + "':"
                                akdbError(expr,insert_columns[0])
                                return False
                # navedene su samo vrijednosti za unos
                elif (len(table_attr_names) < len(insert_attr_values)):
                        print "\nError: too many attibutes, table " + str(token.tableName) + " has " + str(len(table_attr_names)) 
                        return False
                elif (len(table_attr_names) > len(insert_attr_values)):
                        print "\nError: too few attibutes, table " + str(token.tableName) + " has " + str(len(table_attr_names))
                        return False
                else:
                        for index,tip in enumerate(insert_attr_types):
                                if insert_attr_types[index] != int(table_attr_types[index]):
                                        type_name = get_type_name(int(table_attr_types[index]))
                                        print "\nError: type error for attribute '" + insert_attr_names[index] + "':"
                                        akdbError(expr,insert_attr_values[index])
                                        print "Expected: " + type_name
                                        return False
                if(ak47.insert_data_test(table_name,insert_attr_names,insert_attr_values,insert_attr_types) == ak47.EXIT_SUCCESS):
                        return True
                else:
                        return False
                return False
                      
                
## create table command
# @author Franjo Kovacic
class Create_table_command:
        create_table_regex = r"^(?i)create table(\s([a-zA-Z0-9_]+))+?$"
        pattern = None
        matcher = None

        ## matches method
        # checks whether given input matches table_exists command syntax
        def matches(self, input):
                print "matching regex"
                self.pattern = re.compile(self.create_table_regex)
                self.matcher = self.pattern.match(input)
                if (self.matcher != None):
                        message = self.matcher
                else:
                        message = "None"
                
                return message 

        
        # executes the create table expression
        def execute(self):
                print "start parsing.."
                print self.matcher.group(0)
                parser = sql_tokenizer()
                token = parser.AK_parse_create_table(expr)
                # checking syntax
                if isinstance(token, str):
                        print "Error: syntax error in expression"
                        print expr
                        print token
                        return False
                # get table name
                table_name = str(token.tableName)
                # table should not exist yet
                '''
                For some reason, AK_table_exist won't work, it always just exits here, so it's commented out
                if (ak47.AK_table_exist(table_name) == 1):
                        print "Error: table'" + table_name + "' already exist"
                        return False
                '''                
                # get attributes
                '''
                Create table in table.c currently takes only name and type of attributes.
                Parsing works for other attribute properties as well, so it should be added here when possible.
                '''
                create_table_attributes = []
                for attribute in token.attributes:
                        create_table_attributes.append([{'name': str(attribute[0])}, {'type':get_attr_type(str(attribute[1]))}])
                attribute_count = len(create_table_attributes)
                # executing
                try:
                        ak47.AK_create_table(table_name, create_table_attributes, attribute_count)
                        result = "Table created"
                except:
                        result = "Error. Creating table failed."
                return result
