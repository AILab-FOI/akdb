#@file sql_parser.py sql parser, currently only understands INSERT and CREATE TABLE

from pyparsing import *

#literals
createTableLit=CaselessKeyword("create table")
insertLit=CaselessKeyword("insert into")
createUserLit=CaselessKeyword("CREATE USER")
lBracket=Suppress(Literal("("))
rBracket=Suppress(Literal(")"))
valuesLiteral=Suppress(CaselessLiteral("values"))
comma=Suppress(Literal(","))

#identifier
identifier=Word(alphas, alphanums + "_$")
tableName=identifier

#username
userName=Word(alphas, alphanums)

#values
E = CaselessLiteral("e")
arithSign=Word("+-",exact=1)
realNum=Combine(Optional(arithSign)+(Word(nums)+"."+Optional( Word(nums))|("."+Word(nums)))+Optional(E+Optional(arithSign)+Word(nums)))
intNum=Combine(Optional(arithSign)+Word(nums)+Optional(E+Optional("+")+Word(nums)))
value=quotedString | realNum | intNum
valuesList=Group(delimitedList(value))
values=lBracket+valuesList+rBracket

#columns
columnName=identifier
columnNameList=Group(delimitedList(columnName))
columns=lBracket+columnNameList+rBracket

#data types
dataSize=lBracket+intNum+rBracket
floatType=CaselessKeyword("float")
integerType=CaselessKeyword("integer")
varcharType=CaselessKeyword("varchar")+dataSize

#predicate(limited)
binrelop=oneOf(["<", "<=", ">=", ">", "=", "!="])
sqlOR=CaselessKeyword("OR")
sqlAND=CaselessKeyword("AND")
predicate=Forward()
predicate<<columnName+binrelop+value+ZeroOrMore((sqlOR | sqlAND)+predicate)

#constraints in atribute definition
notNullConstraint=CaselessKeyword("NOT NULL")
uniqueConstraint=CaselessKeyword("UNIQUE")
primaryKeyConstraint=CaselessKeyword("PRIMARY KEY")
defaultConstraint=CaselessKeyword("DEFAULT")+value
foreignKeyConstraint=CaselessKeyword("FOREIGN KEY REFERENCES")+tableName+columns
checkConstraint=Group(CaselessKeyword("CHECK")+predicate)
constraint=notNullConstraint | uniqueConstraint | primaryKeyConstraint |checkConstraint
constraints=ZeroOrMore(constraint)

#constraint at the end 
pkUnique=uniqueConstraint | primaryKeyConstraint
pkUniqueEConstraint=pkUnique+columns
foreignKeyEConstraint=CaselessKeyword("FOREIGN KEY")+columns+CaselessKeyword("REFERENCES")+tableName+lBracket+columnName+rBracket
checkEConstraint="ERROR"
endConstraint=Group(CaselessKeyword("CONSTRAINT")+identifier+(pkUniqueEConstraint | checkEConstraint | foreignKeyEConstraint))

endConstraints=Group(delimitedList(endConstraint))

#atributes (create table)
atributeType=floatType | integerType | varcharType
atributeDefinition=Group(identifier+atributeType+constraints)
atributeDefinitionList=Group(delimitedList(atributeDefinition))
atributes=atributeDefinitionList

#constraints in user creation
c0=CaselessKeyword("WITH")
c1=CaselessKeyword("PASSWORD") + value.setResultsName("PASSWORD")
c2=CaselessKeyword("CREATEDB").setResultsName("CREATEDB") ^ CaselessKeyword("NOCREATEDB").setResultsName("NOCREATEDB")
c4=CaselessKeyword("CREATEUSER").setResultsName("CREATEUSER") ^ CaselessKeyword("NOCREATEUSER").setResultsName("NOCREATEUSER")
c6=CaselessKeyword("IN GROUP") + value.setResultsName("IN GROUP")
c7=CaselessKeyword("VALID UNTIL")

createConstraints = OneOrMore ((c1 | c2 | c4 | c6))

#INSERT INTO 
insert=insertLit.setResultsName("commandName")+\
        Optional(columns).setResultsName("columns")+\
        tableName.setResultsName("tableName")+\
        valuesLiteral+\
        values.setResultsName("columnValues")+stringEnd

#CREATE TABLE 
createTable=createTableLit.setResultsName("commandName")+\
             tableName.setResultsName("tableName")+\
             lBracket+\
             atributes.setResultsName("atriburtes")+\
             Optional(comma+endConstraints).setResultsName("endConstraints")+\
             rBracket+stringEnd
             
#CREATE USER  
creteUser = createUserLit.setResultsName("commandName")+\
             userName.setResultsName("USERNAME")+\
             Optional(c0) +\
             Optional(createConstraints).setResultsName("createConstraints")+\
             stringEnd

#add other commands with |
sqlGrammar= insert | createTable | creteUser

def AKparseSql(command):
    '''
    @author Matej Vidakovic
    @brief sql parsing
    @param command sql command as string
    @return if command is successfully parsed returns list of tokens, else returns error message as string 
    '''
    try:
        tokens=sqlGrammar.parseString(command)
        return tokens
    #error msg usually not very useful
    except ParseException, err:
        return "Syntax error at char "+str(err.loc)+" "+err.msg

create_user_test_strings = ["create user korisnik with createdb CREATEUSER", "create user korisnik createdb CREATEUSER", "create user korisnik with createdb CREATEUSER password 'lozinka' in group 'grupa'", "create user korisnik with createdb CREATEUSER in group 'grupa'", "create user korisnik with createdb nocreatedb CREATEUSER in group 'grupa'", "CREATE USER manuel WITH PASSWORD 'jw8s0F4' CREATEDB", "asdasd"]

def test_create_user(string):
    a = AKparseSql(string)
    if type(a) == str:
        print
        print "Invalid CREATE USER query"
        return
    print
    print string
    options = a.keys()
    options.remove("commandName")
    options.remove("createConstraints")
    if (options.count("CREATEDB") == 1 and options.count("NOCREATEDB") == 1) or (options.count("CREATEUSER") == 1 and options.count("NOCREATEUSER") == 1):
        print
        print "Invalid CREATE USER query"
        return		
    print "Options: "
    print options
    print "Username =", a["USERNAME"]
    if "IN GROUP" in a.keys():
        print "Group =", eval(a["IN GROUP"])
    if "PASSWORD" in a.keys():
        print "Password =", eval(a["PASSWORD"])
        
map(test_create_user, create_user_test_strings)