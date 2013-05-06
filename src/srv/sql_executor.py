from sql_tokenizer import *
import time

# Helper function to determine number type

def is_numeric(lit):
    'Return value of numeric literal string or ValueError exception'
 
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

def attr_type(value):

	integer = type(is_numeric(value)) == int
	decimal = type(is_numeric(value)) == float
	varchar = isinstance(value,str)
	date = is_date(value.replace("'",""))
	datetime = is_datetime(value.replace("'",""))
	time = is_time(value.replace("'",""))
	boolean = is_bool(value.replace("'",""))

	if integer:
		return "INT"
	elif decimal:
		return "FLOAT"
	elif date:
		return "DATE"
	elif datetime:
		return "DATETIME"
	elif time:
		return "TIME"
	elif boolean:
		return "BOOL"
	elif varchar:
		return "VARCHAR"
	else:
		print "UNDEFINED"

class sql_executor:

	# Insert function

	def insert(self,expr):
		parser = sql_tokenizer()
		print "\n---------------------------------INSERT INTO test---------------------------------"
		token = parser.AK_parse_insert_into(expr)

		attr_types = []
		for index,value in enumerate(token.columnValues[0]):
			token.columnValues[0][index] = value.replace("'","")
			tip = attr_type(value)
			print tip + ": " + value
			attr_types.append("TYPE_" + tip)

		print "tipovi atributa = " + str(attr_types)

		if isinstance(token, str):
			print "Error: " + token
		else:
			print "U tablicu " + token.tableName + " ubaci " + str(token.columnValues[0])


        


kontiki = sql_executor()

#kontiki.insert("INSERT INTO student(ime, prezime, studij) VALUES ('pero', 'peric', 'informatika')")
kontiki.insert("INSERT INTO test VALUES (11, 22.2, 'neki tekst','2013-05-06','2013-05-06 22:55:55','2013-05-06 22:55:55.2600','23:55:00','false')")
