import sys
sys.path.append('../swig/')
import kalashnikovDB as ak47
from sql_executor import *

ak47.AK_inflate_config()
ak47.AK_init_disk_manager()
ak47.AK_memoman_init()


bulbasaur = sql_executor()

student_attr_name = ["id_student", "firstname", "lastname", "year", "weight"]
student_attr_type = [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_VARCHAR, ak47.TYPE_INT, ak47.TYPE_FLOAT]
student_attr_values = ['2','Pero','Peric','1991','100.2']


ak47.create_header_test("student", student_attr_name, student_attr_type)

print "\n------------------------------------ INSERT INTO TEST ------------------------------------\n"

print "pero: " + str(bulbasaur.insert("INSERT INTO student VALUES ('2','Pero','Peric','1991','100.2')"))
print "ivo: " + str(bulbasaur.insert("INSERT INTO student(id_student,firstname,lastname, year, weight) VALUES ( '3','Ivo','Ivic','2002','90.2')"))

ak47.AK_print_table("student")
