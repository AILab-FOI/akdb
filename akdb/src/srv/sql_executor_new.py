#!/usr/bin/env python2

"""
This version of sql_executor is not usable until kalashnikovDB package is refactored (see issue #188).

All functions that implement certain sql command need to be decorated with Command decorator.
"""

import re
import sys
import time

sys.path.append('../swig/')
import kalashnikovDB as ak47
from sql_tokenizer import *


"""
Saves decorated functions in a dictionary. In syntax regex, command MUST be in first group!
commands = { 
    command1:{"call":<__call__>, "syntax":"^syntaxRegex$", "usage":"usage"},
    command2:{"call":<__call__>, "syntax":"^syntaxRegex$", "usage":"usage"},
    command3:{
        subcommand1:{"call":<__call__>, "syntax":"^syntaxRegex$", "usage":"usage"},
        subcommand2:{"call":<__call__>, "syntax":"^syntaxRegex$", "usage":"usage"},
        ...
    }
    ...
}
"""
class Command:
    commands = {}

    # Note: the command part of syntax needs to be in a group. If '\' is part of syntax, use '/' instead.
    def __init__(self, syntax, parser, usage=""):
        self.usage = usage
        self.syntax = syntax
        self.parser = parser
    
    def __call__(self, func, *args, **kwargs):
        # Searches for command part of syntax and splits it into list
        key = re.search("\((.*?)\)", self.syntax).group(1).split() 

        # Creates tree entry for command
        hierarchy = {}
        tmp = hierarchy
        while len(key) > 1:
            subkey = key.pop(0)
            tmp[subkey] = {}
            tmp = tmp[subkey]

        key = key[0] #.encode().decode("string_escape")
        tmp[key] = {"call":func, "syntax":self.syntax, "parser":self.parser, "usage":self.usage}

        # Merges newly created entry with the main tree
        merge_dicts(Command.commands, hierarchy)

        def wrapper(*args, **kwargs):
            func(*args, **kwargs)
        return wrapper

# Merges d2 into d1
def merge_dicts(d1, d2):
    for key in d2:
        if key in d1:
            if isinstance(d1[key], dict) and isinstance(d2[key], dict):
                merge_dicts(d1[key], d2[key])
        else:
            d1[key] = d2[key]
    return d1

# Searches for command in a dictionary and then returns its entry
def find_command(sql_string):
    cmds_tmp = Command.commands
    sql = sql_string.split()
    for index,word in enumerate(sql):
        if word in cmds_tmp:
            # if command is not callable, i.e. if command requires subcommand
            if "call" not in cmds_tmp[word]:
                cmds_tmp = cmds_tmp[word]
            else:
                return cmds_tmp[word]
        else:
            raise NameError("Command '%s' does not exist." %' '.join(sql[:index+1]))
    return None

def execute_command(sql_string):
    cmd = find_command(sql_string)
    if cmd is not None:
        if re.match(cmd["syntax"], sql_string):
            kwargs = cmd["parser"](sql_string)
            return cmd["call"](**kwargs)
    else:
        return help()

def print_tree(tree, _level=0):
    result = ""
    for c,v in tree.items():
        result += ' '+'  +--'*_level+c + "\n"
        if "call" not in v:
            result += print_tree(v, _level+1)
    return result

def parse_help(sql):
    kwargs = {}
    words = sql.split()
    if len(words) > 1: kwargs["cmd"] = ' '.join(words[1:])
    else: kwargs["cmd"] = None
    return kwargs

@Command('^(help)( .+)* *$', parse_help, "Usage: help [command]")
def help(cmd=None):
    if cmd is None:
        result = "Commands:\n%s" %print_tree(Command.commands)
    else:
        tmp = Command.commands
        cmd = cmd.split()
        for i in cmd:
            if i in tmp:
                tmp = tmp[i]
            else:
                return "Command '%s' not found\n" %i
        if "usage" in tmp:
            result = tmp["usage"]
        else:
            result = print_tree(tmp)
    return result.replace("/", "\\")
    
# Temporary function - until implemented in kalashnikovDB package
def parse_print_table(sql):
    kwargs = {}
    words = sql.split()
    kwargs["table_name"] = words[1]
    return kwargs

@Command(r'^(/p)( [A-Za-z0-9_]+ *$)', parse_print_table, "Prints table\nUsage: \\p <table_name>")
def print_table(table_name):
    ak47.AK_print_table(table_name)

# Temporary function - until implemented in kalashnikovDB package
def parse_print_table_details(sql):
    kwargs = {}
    words = sql.split()
    kwargs["table_name"] = words[1]
    return kwargs

@Command(r'^(/d)( [A-Za-z0-9_]+ *$)', parse_print_table_details, "Prints table details\nUsage: \\d <table_name>")
def print_table_details(table_name):
    result = "Number of attributes: %s\n" %str(ak47.AK_num_attr(table_name))
    result += "Number od records: %s\n" %str(ak47.AK_get_num_records(table_name))
    return result

# Temporary function - until implemented in kalashnikovDB package
def parse_create_index(sql):
    return {}

@Command('^(CREATE INDEX)( [A-Za-z0-9_]+ *$))', parse_create_index, "Usage: CREATE INDEX <index_name>")
def create_index():
    return

# Temporary function - until implemented in kalashnikovDB package
def parse_create_table(sql):
    return {}

@Command('^(CREATE TABLE)( [A-Za-z0-9_]+ *$)', parse_create_table, "Usage: CREATE TABLE <table_name>")
def create_table():
    return

ak47.AK_inflate_config()
ak47.AK_init_disk_manager()
ak47.AK_memoman_init()

if __name__=='__main__':
    print Command.commands
    while True:
        cmd = raw_input("#> ").replace('\\', '/')
        print execute_command(cmd)
