#!/usr/bin/env python

from sql_tokenizer import *
import time

import sys
sys.path.append('../swig/')
import kalashnikovDB as ak47
import test

# Helper function to determine number type

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

# Helper function to determine if the string of DATE type

def is_date(lit):
	try:
	  time.strptime(lit, '%Y-%m-%d')
	  return True
	except ValueError:
	  return False

# Helper function to determine if the string of DATETIME type

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

# Helper function to determine if the string of TIME type

def is_time(lit):
	try:
		time.strptime(lit, '%H:%M:%S')
		return True
	except ValueError:
		return False

def is_bool(lit):
	return lit.lower() in ("true","false")

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

class sql_executor:

	# Insert function

	def insert(self,expr):

		parser = sql_tokenizer()
		token = parser.AK_parse_insert_into(expr)
		attr_values = map(lambda x: x.replace("'",""),list(token.columnValues[0]))
		attr_types = map(lambda x: get_attr_type(x.replace("'","")),list(token.columnValues[0]))
		if(token.columns):
			attr_names = list(token.columns[0])
		else:
			attr_names = str(ak47.AK_rel_eq_get_atrributes_char("student")).split(";")
		'''
		print attr_names
		print attr_values
		print attr_types
		'''
		ak47.insert_data_test(str(token.tableName),attr_names,attr_values,attr_types)



# Testiranje INSERT INTO

ak47.AK_inflate_config()
ak47.AK_init_disk_manager()
ak47.AK_memoman_init()

student_attr_name = ["id_student", "firstname", "lastname", "year", "weight"]
student_attr_type = [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_VARCHAR, ak47.TYPE_INT, ak47.TYPE_FLOAT]
student_attr_value_1 = ["1", "Collin", "Channell", "1990", "100.1"]

ak47.create_header_test("student", student_attr_name, student_attr_type)

charizard = sql_executor()

# Radi za pojedinacne unose ali ne i za vise odjednom

charizard.insert("INSERT INTO student VALUES ('2', 'Pero', 'Peric','1991','100.2')")
#charizard.insert("INSERT INTO student(id_student, firstname, lastname, year, weight) VALUES ('3', 'Ivo', 'Ivic','2002','90.2')")

ak47.AK_print_table("student")
