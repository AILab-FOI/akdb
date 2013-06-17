#import sys
#sys.path.append('../swig/')
#import kalashnikovDB as ak47
from sql_executor import *
'''
ak47.AK_inflate_config()
ak47.AK_init_disk_manager()
ak47.AK_memoman_init()

bulbasaur = sql_executor()

student_attr_name = ["id_student", "firstname", "lastname", "year", "weight"]
student_attr_type = [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_VARCHAR, ak47.TYPE_INT, ak47.TYPE_FLOAT]
student_attr_values = ['2','Pero','Peric','1991','100.2']

ak47.create_header_test("student", student_attr_name, student_attr_type)

print "\n------------------------------------ INSERT INTO TEST ------------------------------------\n"

print "1 - " + str(bulbasaur.insert("INSERT INTO students VALUES ('1s','Pero','Peric','1991','100.2')"))
print "2 - " + str(bulbasaur.insert("INSERT INTO student(id_student, firstnames, lastname, year, weight) VALUES ( '2','Ivo','Ivic','2002','90.2')"))

print "\n------------------------------------ SELECT TEST ------------------------------------\n"

print "1 - " + str(select("SELECT * FROM student WHERE id_student = 1"))

print "\n------------------------------------ UPDATE TEST ------------------------------------\n"

print "1 - " + str(update("UPDATE student SET weight = 80 WHERE id_student = 1"))

print "\n------------------------------------ PRINT TABLE STUDENT ------------------------------------\n"

ak47.AK_print_table("student")
'''
'''

#------------test executor for create sequence by Danko Sacer----------------
print "\n------------Testing executor for create sequence by Danko Sacer----------------\n"

print "\nCreate sequence brojac_1 start with 1 increment by 2 minvalue 0 maxvalue 100 cycle 1"

initialize()
test = sql_sequence()

if(test.create_sequence("Create sequence brojac_1 start with 1 increment by 2 minvalue 0 maxvalue 100 cycle 1")==True):
	print "Everything was fine\n"
else:
	print "Something's gone wrong. Unable to create sequence!\n"

ak47.AK_print_table("AK_sequence")

# next query has overflow buffer error

#print "\nSljedeci upit\n"
#test.create_sequence("Create sequence brojac_2 start with 1 increment by 2 minvalue 0 maxvalue 100")
#ak47.AK_print_table("AK_sequence")

'''

# create table test by Franjo Kovacic
test = Create_table_command()
commands = ["CREATE TABLE tablica (var1 INT, var2 INT)", "CREATE TABLE tabla1 (v2 INT, v4 TEXT, v11 INT AUTO_INCREMENT)"]
for command in commands:
        test.expr = command
        print test.execute()


# create index test by Franjo Kovacic
test = Create_index_command()
commands = ["CREATE INDEX Pindex ON tablica ( stupac1, stupac2 ) USING Btree",\
                    "create index Pindex on tablica ( stupac1 ) USING Btree",\
                    "create index Pindex on tablica ( stupac1, stupac2 ) USING Hash"]
for command in commands:
        test.expr = command
        print test.execute()

# create trigger test by Franjo Kovacic
test = Create_trigger_command()
commands=["CREATE TRIGGER prihvat_veze AFTER INSERT ON veza FOR ROW EXECUTE PROCEDURE veza_prihvacena()",\
                  "CREATE TRIGGER prihvat_veze BEFORE  DELETE OR INSERT ON veza EXECUTE PROCEDURE veza_prihvacena()",\
                  "CREATE TRIGGER prihvat_veze BEFORE DELETE OR INSERT ON veza FOR EACH STATEMENT EXECUTE PROCEDURE veza_prihvacena(2,3,'data')",\
                  "CREATE TRIGGER prihvat_veze AFTER DELETE OR INSERT OR UPDATE ON veza FOR EACH STATEMENT EXECUTE PROCEDURE veza_prihvacena(2,10.5,'data')"]
for command in commands:
        test.expr = command
        print test.execute()

