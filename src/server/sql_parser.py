#@file sql_parser.py sql parser, currently only understands INSERT and CREATE TABLE
#
# autor: Matej Vidakovic
# dopunio: Kresimir Ivkovic
#
# 2012-06-01 izmjene:
# ispravljen insert into
# dodan select
# dodan delete
# dodan where

from pyparsing import *

#literals
createTableLit=CaselessKeyword("create table")
insertLit=CaselessKeyword("insert into")
deleteLit=CaselessKeyword("delete")
selectLit=CaselessKeyword("select")
fromLit=CaselessKeyword("from")
usingLit=CaselessKeyword("using")
whereLit=CaselessKeyword("where")
lBracket=Suppress(Literal("("))
rBracket=Suppress(Literal(")"))
valuesLiteral=Suppress(CaselessLiteral("values"))
comma=Suppress(Literal(","))

#identifier
identifier=Word(alphas, alphanums + "_$")
tableName=identifier

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
columnNameListSelect=Group(delimitedList(columnName)) | '*'
columns=lBracket+columnNameList+rBracket

#data types
dataSize=lBracket+intNum+rBracket
floatType=CaselessKeyword("float")
integerType=CaselessKeyword("integer")
varcharType=CaselessKeyword("varchar")+dataSize

#predicate(limited)
binrelop=oneOf(["<", "<=", ">=", ">", "=", "!="])
sqlIN=CaselessKeyword("IN")
sqlBetween=CaselessKeyword("BETWEEN")
sqlLike=CaselessKeyword("LIKE")
sqlOR=CaselessKeyword("OR")
sqlAND=CaselessKeyword("AND")
predicate=Forward()
predicate<<columnName+binrelop+value+ZeroOrMore((sqlOR | sqlAND | sqlIN | sqlBetween | sqlLike)+predicate)

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

#INSERT INTO 
insert=insertLit.setResultsName("commandName")+\
        tableName.setResultsName("tableName")+\
        Optional(columns).setResultsName("columns")+\
        valuesLiteral+\
        values.setResultsName("columnValues")+stringEnd

#CREATE TABLE 
createTable=createTableLit.setResultsName("commandName")+\
             tableName.setResultsName("tableName")+\
             lBracket+\
             atributes.setResultsName("attributes")+\
             Optional(comma+endConstraints).setResultsName("endConstraints")+\
             rBracket+stringEnd

#WHERE
whereExpression=Forward()
select=Forward()
selectInner=Forward()


#### za ugnijezdjene select upite unutar where-a treba staviti da select ide unutar liste (u uglate zagrade)
whereCond = Group(
				(columnName.setResultsName("whereCondLval") + binrelop.setResultsName("whereOp") + columnName.setResultsName("whereCondRval")) |
				(columnName.setResultsName("whereCondLval") + binrelop.setResultsName("whereOp") + value.setResultsName("whereCondRval")) |
				(columnName.setResultsName("whereCondLval") + sqlIN.setResultsName("whereOp") + values.setResultsName("whereCondRval")) |
				(columnName.setResultsName("whereCondLval") + sqlIN.setResultsName("whereOp") + selectInner.setResultsName("whereCondRval"))
			)

where = whereLit.setResultsName("where")+\
		whereExpression.setResultsName("whereExpression")

whereExpression << whereCond + ZeroOrMore( ( sqlOR | sqlAND ) + whereExpression )

            
#SELECT
selectStatement=selectLit.setResultsName("commandName")+\
		columnNameListSelect.setResultsName("attributes")+\
		fromLit.setResultsName("from")+\
		tableName.setResultsName("tableName")+\
		Optional(where.setResultsName("condition"))

select << selectStatement + stringEnd
selectInner << lBracket+selectStatement+rBracket

#DELETE
deleteFrom=deleteLit.setResultsName("commandName")+\
		fromLit.setResultsName("from")+\
		tableName.setResultsName("tableName")+\
		Optional(usingLit.setResultsName("using")+tableName.setResultsName("usingTable"))+\
		Optional(where.setResultsName("condition"))+stringEnd

#add other commands with |
sqlGrammar= insert | createTable | select | deleteFrom

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

if __name__ == '__main__':
	print AKparseSql("insert into table (a, b, c) values(1, 2, 3)");
	print AKparseSql("insert into table values(1, 2, 3)");
	print AKparseSql("select * from tablica");
	print AKparseSql("select a from tablica");
	print AKparseSql("select a, b, c from tablica");
	print AKparseSql("delete from tablica");
	print AKparseSql("delete from tablica using b where a = b");
	print AKparseSql("select a,b,c from a where b = c");
	print AKparseSql("select * from b where b = c and c = 2 or x > 0");
	print AKparseSql("select * from b where b in ('1','2')");
	print AKparseSql("select * from b where b in (select * from c where c = 2)");
	print AKparseSql("delete from b where a in (select c from d where c in (select e from e))");
