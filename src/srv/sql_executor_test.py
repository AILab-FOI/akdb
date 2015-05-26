import sys
from sql_executor import *

#sql_executor tests by Marko Sinko, updated by Matija Jurman and Tomislav Ilisevic


print "\n---------------------------IS NUMERIC TEST -----------------------------\n"
commands=["1","a","2","b","3","4","c"]
print "Polje sa testnim podacima:", commands
for command in commands:
	broj = command
	print is_numeric(broj)
print "\n--------------------------IS NUMERIC TEST END ---------------------------\n"




print "\n---------------------------IS DATE TEST -----------------------------\n"
commands=["2011-12-12","21234","2013-05-21","2011.23.45"]
print "Polje sa testnim podacima:", commands
for command in commands:
	datum = command
	print is_date(datum)
print "\n--------------------------IS DATE TEST END ---------------------------\n"




print "\n---------------------------IS DATETIME TEST -----------------------------\n"
commands=["12:12:12","21234","13:21:56","2011.23.45"]
print "Polje sa testnim podacima:", commands
for command in commands:
	datum = command
	print is_datetime(datum)
print "\n--------------------------IS DATETIME TEST END ---------------------------\n"




print "\n---------------------------IS TIME TEST -----------------------------\n"
commands=["12:12:12","21234","13:21:56","2011.23.45"]
print "Polje sa testnim podacima:", commands
for command in commands:
	datum = command
	print is_time(datum)
print "\n--------------------------IS TIME TEST END ---------------------------\n"




print "\n---------------------------GET ATTRIBUTE TYPE TEST -----------------------------\n"
commands=["1","1.2","a","#"]
print "Polje sa testnim podacima:", commands
for command in commands:
	att_type = command
	print get_attr_type(att_type)

print "\n---------------------------GET ATTRIBUTE TYPE TEST END -----------------------------\n"






#inicijalizacija te ucitavanje sql_executor funkcije
initialize()
executor = sql_executor()

#kreiranje atributa:id_student, ime, prezime, godina_rodenja i prosjek
student_attr_name = ["id_student", "ime", "prezime", "godina_rodenja", "prosjek"]

#definiranje tipa svakog od atributa
student_attr_type = [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_VARCHAR, ak47.TYPE_INT, ak47.TYPE_FLOAT]

#kreiranje tablice pod imenom student sa gore navedenim atributima te tipovima atributa
ak47.create_header_test("student", student_attr_name, student_attr_type)





print "\n----------------------------- TESTNA TABLIC------------------------\n"

#ispis sadrzaja kreirane tablice student
ak47.AK_print_table("student")
print "\n---------------------------- TESTNA TABLIC END------------------------\n"




print "\n---------------------------- INSERT INTO TEST -----------------------------\n"

#Kreiranje upita koji se proslijeduje sql_executor klasi metodi insert
#Upiti se izvrsavaju, te insert radi u potpunosti
print "Prvi upit: INSERT INTO student VALUES ('1','Marko','Sinko','1990','2.0') " + str(executor.insert("INSERT INTO student VALUES ('1','Marko','Sinko','1990','2.0')"))
print "\n"
print "Drugi upit: INSERT INTO student(id_student, ime, prezime, godina_rodenja, prosjek) VALUES ( '2','Marinko','Radic','1991','3.3') " + str(executor.insert("INSERT INTO student(id_student, ime, prezime, godina_rodenja, prosjek) VALUES ( '2','Marinko','Radic','1991','3.3')"))
print "\n"

#Ispis nakon izvrsenih upita
ak47.AK_print_table("student")
print "\n--------------------------- INSERT INTO TEST END -------------------------------\n"





print "\n---------------------------CREATE GROUP TEST -----------------------------\n"
#Za ovaj test nisu implementirani tokeni, potrebno je kreirati Create_group_command() token
#za daljnje testiranje, potrebna implementacija, potrebno provjeriti AK_group_add() funkciju
#AK_group_add(tokens.groupname, ak47.NEW_ID)--cudni ulazni parametri u funkciju? groupname-NEW_ID?
print "\n---------------------------CREATE GROUP TEST END -----------------------------\n"





print "\n---------------------------CREATE GRANT TEST -----------------------------\n"
grant = Grant_command()
grant.command = "GRANT INSERT ON student TO Marko"
#Tokenizer radi u redu, treba provjeriti u klasi Grant_command() kako se izvrsava metoda
#grant.execute() jer ne prihvaca dobro naredbu sto se moze vidjeti po izlazu kojeg dobijemo
#izvrsavanjem grant.execute() metode, potrebno provjeriti implementaciju AK_grant_privilege_user(user, table, privilege) funkcije
print grant.execute()

print "\n---------------------------CREATE GRANT TEST END -----------------------------\n"




print "\n------------------------------ SELECT TEST -------------------------------\n"
#Napomena, SELECT TEST uvijek vraca samo cijelu tablicu, WHERE klauzula ne radi, najvjerojatnije
#ne radi dobro funkcija selection_test(), trebalo bi provjeriti u nizoj razini implementaciju selecta ili
#potrebno testirati AK_parse_where() tokenizaciju
select = Select_command()
print str(select.execute("SELECT * FROM student"))
print "\n------------------------------ SELECT TEST END ------------------------------\n"





print "\n------------------------------- UPDATE TEST --------------------------------\n"
#Nakon commita 553 javlja se dodatni error za funkciju: Ak_Insert_New_Element_For_Update()
#Moguce da nesto nije u redu sa AK_update_Row(table, column1, column2, key, new_value) funkcijom za redm takoder koristiti AK_parse_where() tokenizaciju te ju treba provjeriti 
update = Update_command()
print str(update.execute("UPDATE student SET prosjek = 2.200 WHERE id_student = 1"))
ak47.AK_print_table("student")
print "\n------------------------------- UPDATE TEST END ------------------------------\n"



print "\n------------------------------- DELETE TEST --------------------------------\n"

delete = Delete_command()

print "\nDELETE FROM student WHERE id_student = 1"
delete.execute("DELETE FROM student WHERE id_student = 1")

ak47.AK_print_table("student")
print "\n------------------------------- DELETE TEST END ------------------------------\n"






print "\n------------------------------- Create sequence TEST --------------------------------\n"


print "\nCreate sequence brojac_1 start with 1 increment by 2 minvalue 0 maxvalue 100 cycle 1"
sequence = Create_sequence_command()

#Takoder sljedeca metoda ne radi kako treba te bi bilo potrebno provjeriti funkciju AK_sequence_add
sequence.execute("Create sequence brojac_1 start with 1 increment by 2 minvalue 0 maxvalue 100 cycle 1")

#sljedeci upit imao je overflow buffer error ali prilikom mojeg testiranja radi normalno

print "\nSljedeci upit:\n"

sequence.execute("Create sequence brojac_2 start with 1 increment by 2 minvalue 0 maxvalue 100")
print "\n---------------------------- Create sequence TEST END ----------------------------------\n"

print "\n------------------------------- Alter sequence TEST --------------------------------\n"


print "\nAlter sequence brojac_1 start with 2 increment by 5 minvalue 1 maxvalue 100 cycle 1"
sequence = Alter_sequence_command()

sequence.execute("Alter sequence brojac_1 start with 2 increment by 5 minvalue 1 maxvalue 100 cycle 1")

print "\n---------------------------- Alter sequence TEST END ----------------------------------\n"






print "\n--------------------------------- CREATE TABLE TEST --------------------------------\n"
create = Create_table_command()
create.expr = "CREATE TABLE tablica (var1 INT, var2 INT)"
#Create table ne radi
#Potrebna reimplementacija tokenizacije AK_parse_create_table() jer ne sadrzi AK_TYPE_INT,AK_TYPE_VARCHAR tipove podataka koji su potrebni za dodavanje atributa u tablicu, INT se ne koristi, potrebno koristiti ovakvu strukturu: [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_VARCHAR, ak47.TYPE_INT, ak47.TYPE_FLOAT] kao u gore navedenom primjeru
ak47.AK_print_table("tablica")
print "\n--------------------------------- CREATE TABLE TEST END ------------------------------\n"



print "\n--------------------------------- ALTER TABLE TEST --------------------------------\n"
alter = Alter_table_command()

print "\nALTER TABLE tablica DROP column ime1"

alter.expr = "ALTER TABLE tablica DROP column ime1"
alter.execute(alter.expr)

ak47.AK_print_table("tablica")
print "\n--------------------------------- ALTER TABLE TEST END ------------------------------\n"





print "\n--------------------------------- CREATE INDEX TEST ---------------------------------\n"
index = Create_index_command()
commands = ["CREATE INDEX Pindex ON student ( stupac1, stupac2 ) USING Btree",\
                    "create index Pindex on student ( stupac1, stupac2 ) USING Hash"]
for command in commands:
        index.expr = command
#U klasi Create_index_command njezina metoda execute ne radi ispravno, tokeni su testirani te rade 
#ispravno, potrebno je provjeriti AK_create_index funkciju
        print index.execute()
print "\n--------------------------------- CREATE INDEX TEST END--------------------------------\n"




print "\n--------------------------------- ALTER INDEX TEST ---------------------------------\n"
index = Alter_index_command()

print "\nALTER INDEX Pindex RENAME TO ZZindex"

index.expr = "ALTER INDEX Pindex RENAME TO ZZindex"

print index.execute("ALTER INDEX Pindex RENAME TO ZZindex")

print "\n--------------------------------- ALTER INDEX TEST END--------------------------------\n"






print "\n------------------------------- CREATE TRIGGER TEST -------------------------------\n"
trigger = Create_trigger_command()
commands=["CREATE TRIGGER prihvat_veze AFTER INSERT ON veza FOR ROW EXECUTE PROCEDURE veza_prihvacena()",\
          "CREATE TRIGGER prihvat_veze BEFORE  DELETE OR INSERT ON veza EXECUTE PROCEDURE veza_prihvacena()",\
          "CREATE TRIGGER prihvat_veze BEFORE DELETE OR INSERT ON veza FOR EACH STATEMENT EXECUTE PROCEDURE veza_prihvacena(2,3,'data')",\
          "CREATE TRIGGER prihvat_veze AFTER DELETE OR INSERT OR UPDATE ON veza FOR EACH STATEMENT EXECUTE PROCEDURE veza_prihvacena(2,10.5,'data')"]
for command in commands:
        trigger.expr = command
#U klasi Create_trigger_command njezina metoda execute ne radi ispravno, tokeni su testirani te rade 
#ispravno, potrebno je provjeriti funkciju AK_trigger_add - treci argument te liste javlja error: TypeError: in method 'AK_trigger_add', argument 3 of type 'AK_list *'
        print trigger.execute()
print "\n------------------------------- CREATE TRIGGER TEST END --------------------------\n"




print "\n------------------------------------ DROP TEST ------------------------------------\n"

ak47.AK_print_table("student")

drop = Drop_command()
drop.expr = "DROP TABLE student"
#Drop test radi za dropanje tablica, potrebno ispraviti implementacije gore navedenih testova kako bi se testirali ostali dropovi ali trebali bi svi raditi, tokenizacija radi ispravno
print drop.execute()
ak47.AK_print_table("student")
print "\n------------------------------------DROP TEST END ------------------------------------\n"



print "\n------------------------------------ CREATE VIEW TEST ------------------------------------\n"

print "\nCREATE VIEW pogledStudent AS SELECT * FROM student"
create_view = Create_view_command()
create_view.expr = "CREATE VIEW pogledStudent AS SELECT * FROM student"

print create_view.execute("CREATE VIEW pogledStudent AS SELECT * FROM student")

print "\n------------------------------------CREATE VIEW TEST END ------------------------------------\n"



print "\n------------------------------------ ALTER VIEW TEST ------------------------------------\n"

print "\nALTER VIEW pogledStudent RENAME TO pStudent"
alter_view = Alter_view_command()
alter_view.expr = "ALTER VIEW pogledStudent RENAME TO pStudent"

print alter_view.execute("ALTER VIEW pogledStudent RENAME TO pStudent")

print "\n------------------------------------ALTER VIEW TEST END ------------------------------------\n"



'''
Potrebno je povezati sljedece tokene sa sql_executorom:
AK_parse_create_user
AK_parse_CreateView -- promijena imena u AK_parse_create_view posto je to neki standard u sql_executor.py datoteci

AK_alter_table  -- nevidim razloga zbog cega se ovo ne zove AK_parse_alter_table jer ce doci do problema 				sa pravom AK_alter_table funkcijom
AK_parse_alter_user
AK_parse_alter_view
AK_parse_alter_index
AK_parse_alter_sequence
'''







