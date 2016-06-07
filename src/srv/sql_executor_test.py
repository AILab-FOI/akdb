import sys
from sql_executor import *

#sql_executor tests by Marko Sinko


print "\n---------------------------IS NUMERIC TEST -----------------------------\n"
commands=["1","a","2","b","3","4","c"]
print "Array with testing data:", commands
for command in commands:
	num = command
	print is_numeric(num)
print "\n--------------------------IS NUMERIC TEST END ---------------------------\n"




print "\n---------------------------IS DATE TEST -----------------------------\n"
commands=["2011-12-12","21234","2013-05-21","2011.23.45"]
print "Array with testing data:", commands
for command in commands:
	date = command
	print is_date(date)
print "\n--------------------------IS DATE TEST END ---------------------------\n"




print "\n---------------------------IS DATETname TEST -----------------------------\n"
commands=["12:12:12","21234","13:21:56","2011.23.45"]
print "Array with testing data:", commands
for command in commands:
	date = command
	print is_datetname(date)
print "\n--------------------------IS DATETname TEST END ---------------------------\n"




print "\n---------------------------IS Tname TEST -----------------------------\n"
commands=["12:12:12","21234","13:21:56","2011.23.45"]
print "Array with testing data:", commands
for command in commands:
	date = command
	print is_tname(date)
print "\n--------------------------IS Tname TEST END ---------------------------\n"




print "\n---------------------------GET ATTRIBUTE TYPE TEST -----------------------------\n"
commands=["1","1.2","a","#"]
print "Array with testing data:", commands
for command in commands:
	att_type = command
	print get_attr_type(att_type)

print "\n---------------------------GET ATTRIBUTE TYPE TEST END -----------------------------\n"






# init and load sql_executor function
initialize()
executor = sql_executor()

# crate attribute: id_student, name, last name, year_of_birth, gradeAvg
student_attr_name = ["id_student", "name", "lastName", "year_of_birth", "gradeAvg"]

# def the type of every attribute
student_attr_type = [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_VARCHAR, ak47.TYPE_INT, ak47.TYPE_FLOAT]

# create table with student name with the attributes above and attribute types
ak47.create_header_test("student", student_attr_name, student_attr_type)





print "\n----------------------------- TEST TABLE------------------------\n"

# PRINT THE CREATED TABLE STUDENT
ak47.AK_print_table("student")
print "\n---------------------------- TEST TABLE END------------------------\n"




print "\n---------------------------- INSERT INTO TEST -----------------------------\n"

# creating the querie that's passed to the sql_executor class, insert method
# execute the queries, do the whole insert
print "First query: INSERT INTO student VALUES ('1','Marko','Sinko','1990','2.0') " + str(executor.insert("INSERT INTO student VALUES ('1','Marko','Sinko','1990','2.0')"))
print "\n"
print "Second query: INSERT INTO student(id_student, name, lastName, year_of_birth, gradeAvg) VALUES ( '2','Marinko','Radic','1991','3.3') " + str(executor.insert("INSERT INTO student(id_student, name, lastName, year_of_birth, gradeAvg) VALUES ( '2','Marinko','Radic','1991','3.3')"))
print "\n"

# print after executing query
ak47.AK_print_table("student")
print "\n--------------------------- INSERT INTO TEST END -------------------------------\n"





print "\n---------------------------CREATE GROUP TEST -----------------------------\n"
# tokens are not implemented for this test, need to create Create_group_command() token
# for further testing, need implementation, need to check AK_group_add() function
# AK_group_add(tokens.groupname, ak47.NEW_ID) -- weird input params?? groupname-NEW_ID??
print "\n---------------------------CREATE GROUP TEST END -----------------------------\n"





print "\n---------------------------CREATE GRANT TEST -----------------------------\n"
grant = Grant_command()
grant.command = "GRANT INSERT ON student TO Marko"
#Tokenizer works well, need to check in the Grand_command() class how the grand.execute method
# is executed becauze it doesn't accept the command like it should. That can be seen from the 
# return value when executing the grand.execute() method, need to check the implementation of
# AK_grant_privilege_user(user, table, privilege) function

print grant.execute()

print "\n---------------------------CREATE GRANT TEST END -----------------------------\n"




print "\n------------------------------ SELECT TEST -------------------------------\n"
# SELECT TEST always returns the whole table, WHERE doesn't work, probably the selection_test() doesn't
# work as well, need to check the implementation in the lower level of the SELECT statement or need to
# test AK_parse_where() tokenizing
select = Select_command()
print str(select.execute("SELECT * FROM student"))
print "\n------------------------------ SELECT TEST END ------------------------------\n"





print "\n------------------------------- UPDATE TEST --------------------------------\n"
# after the 553 commit it raises a new error: Ak_Insert_New_Element_For_Update()
# probably something is wrong with the AK_update_Row(table, column1, column2, key, new_value) function
# also need to check AK_parse_where() tokenizing 
update = Update_command()
print str(update.execute("UPDATE student SET gradeAvg = 2.200 WHERE id_student = 1"))
ak47.AK_print_table("student")
print "\n------------------------------- UPDATE TEST END ------------------------------\n"





print "\n------------------------------- EXECUTOR TEST --------------------------------\n"


print "\nCreate sequence numac_1 start with 1 increment by 2 minvalue 0 maxvalue 100 cycle 1"
#sequence = Create_sequence_command()

# the method above also doesn't work as it should
# also need to check AK_sequence_add function
# sequence.execute("Create sequence numac_1 start with 1 increment by 2 minvalue 0 maxvalue 100 cycle 1")
# this query had an overflow buffer error but in my test it works okay

print "\nSljedeci query:\n"

#sequence.execute("Create sequence numac_2 start with 1 increment by 2 minvalue 0 maxvalue 100")
print "\n---------------------------- EXECUTOR TEST END ----------------------------------\n"





print "\n--------------------------------- CREATE TABLE TEST --------------------------------\n"
create = Create_table_command()
create.expr = "CREATE TABLE tbl (var1 INT, var2 INT)"
# create table doesn't work
# need to reimplement tokenizing of AK_parse_create_table() because it doesn't have AK_TYPE_INT,AK_TYPE_VARCHAR types that are 
# needed for adding attributes to the table
# INT is not used
# need to have this structure: [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_VARCHAR, ak47.TYPE_INT, ak47.TYPE_FLOAT] like in the example above
ak47.AK_print_table("tbl")
print "\n--------------------------------- CREATE TABLE TEST END ------------------------------\n"




print "\n--------------------------------- CREATE INDEX TEST ---------------------------------\n"
index = Create_index_command()
commands = ["CREATE INDEX Pindex ON student ( stupac1, stupac2 ) USING Btree",\
                    "create index Pindex on student ( stupac1, stupac2 ) USING Hash"]
for command in commands:
        index.expr = command
# in the Create_index_command class the execute method doesn't work properly, tokens are tested and they work
# okay, need to check AK_create_index function
        print index.execute()
print "\n--------------------------------- CREATE INDEX TEST END--------------------------------\n"




print "\n------------------------------- CREATE TRIGGER TEST -------------------------------\n"
trigger = Create_trigger_command()
commands=["CREATE TRIGGER accept_conn AFTER INSERT ON conn FOR ROW EXECUTE PROCEDURE conn_accepted()",\
          "CREATE TRIGGER accept_conn BEFORE  DELETE OR INSERT ON conn EXECUTE PROCEDURE conn_accepted()",\
          "CREATE TRIGGER accept_conn BEFORE DELETE OR INSERT ON conn FOR EACH STATEMENT EXECUTE PROCEDURE conn_accepted(2,3,'data')",\
          "CREATE TRIGGER accept_conn AFTER DELETE OR INSERT OR UPDATE ON conn FOR EACH STATEMENT EXECUTE PROCEDURE conn_accepted(2,10.5,'data')"]
for command in commands:
        trigger.expr = command
# in Create_trigger_command class its execute method doesn't work properly. Tokens are tested and they work okay
# need to check AK_trigger_add function - 3rd argument raises error: TypeError: in method 'AK_trigger_add', argument 3 of type 'AK_list *'
        print trigger.execute()
print "\n------------------------------- CREATE TRIGGER TEST END --------------------------\n"




print "\n------------------------------------ DROP TEST ------------------------------------\n"

ak47.AK_print_table("student")

drop = Drop_command()
drop.expr = "DROP TABLE student"
# drop test works, need to correct the implementation of the tests above so that the rest of the drops will be tested, but they should work
# tokenizing works okay
print drop.execute()
ak47.AK_print_table("student")
print "\n------------------------------------DROP TEST END ------------------------------------\n"


'''
Need to connect these tokens with sql_executor:
AK_parse_create_user
AK_parse_CreateView -- change name in AK_parse_create_view because it's some kind of standard in sql_executor.py file

AK_alter_table  -- I can't see a valid reason whe this isn't named like AK_parse_alter_table because it can make some problems with the real AK_alter_table function
AK_parse_alter_user
AK_parse_alter_view
AK_parse_alter_index
AK_parse_alter_sequence
'''