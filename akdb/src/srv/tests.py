#Class that contains functions for automatic client-server communication testing 
#Written by : Marko Vertus
import sys
import time
#Test for creating a table
def CreateTableTest():
    testString="create table test(redak varchar(20)) "
    return testString
#Test for listing out all the commands currently supported
def AllCommandsTest():
    testString="\?"
    return testString
#Test for printing out details on a given table
def TableDetailsTest():
    testString="\d test"
    return testString
#Test for printing out the table
def PrintTableTest():
    testString="\p test"
    return testString
#Test for checking if the table exists or not
def TableExistsTest():
    testString="\t test?"
    return testString
#Test for creating a new index on a table
#Currently not really supported/implemented
def CreateIndexTest():
    testString="create index testniIndex on table test(redak)"
    return testString
#Test for creating a new user with a new password
def CreateUserTest():
    testString="create user testniKorisnik with password testnaLozinka"
    return testString
#Test for creating a new trigger on a table using some function ( currently written as xy)
#Currently not really supported/implemented
#See issue 176 for more detail
def CreateTriggerTest():
    testString="create trigger testniTrigger on test for each row execute xy;"
    return testString
#Test for creating a new sequence 
def CreateSequenceTest():
    testString="create sequence testniSequence start with 1 increment by 1 maxvalue 999 cycle;"
    return testString
#Test for creating a new view for a given table
#Currently not really supported/implemented
def CreateViewTest():
    testString="create view comedies as select * from films where kind='comedy'"
    return testString
#Test for creating a new function
#Currently not really supported/implemented
def CreateFunctionTest():
    testString="create function testnaFunkcija(integer,integer) returns integer as 'select $1+$2' language sql "
    return testString
#Test for altering an existing index
#Currently not really supported/implemented
def AlterIndexTest():
    testString="alter index testniIndex rename to testniIndex2"
    return testString
#Test for altering an existing table
#Currently not really supported/implemented
def AlterTableTest():
    testString="alter table test rename to test2"
    return testString
#Test for altering an existing sequence
#Currently not really supported/implemented
def AlterSequenceTest():
    testString="alter sequence  testniSequence rename to testniSequence2"
    return testString
#Test for altering an existing view
#Currently not really supported/implemented
def AlterViewTest():
    testString="alter view comedies rename to horror"
    return testString
#Test for 'select' operator
def SelectTest():
    testString="select * from test"
    return testString
#Test for 'update' operator
def UpdateTest():
    testString="update test set redak=1 where redak=2"
    return testString
#Test for 'insert' operator
def InsertTest():
    testString="insert into test(redak) values('test')"
    return testString
#Testing for granting permissions on tables
def GrantTest():
    testString="grant insert on test to public"
    return testString
#Test for dropping a table
def DropTest():
    testString="drop table test"
    return testString
#Test for starting a begin-end block
#Currently not really supported/implemented
def BeginTest():
    testString="begin"
    return testString
#Test for writing out history of actions
def HistoryTest():
    testString="history"
    return testString
