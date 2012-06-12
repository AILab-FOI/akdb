#@file sql_tokenizer.py - Parsing commands

from pyparsing import *

class sql_tokenizer:

  def AK_parse_grant(self, string):
      '''
      @author Boris Kisic
      @brief sql parsing of GRANT command
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string 
      '''
      ident = Word( alphas, alphanums + "_$" ).setName("identifier")
      grantToken = Keyword("grant", caseless=True)
      onToken   = Keyword("on", caseless=True)
      toToken   = Keyword("to", caseless=True)
      withToken   = Keyword("with", caseless=True)
      privilege = delimitedList(ident, ",", combine=True)
      privilegeList = Group(delimitedList(privilege))
      table = delimitedList(ident, ",", combine=True)
      tableList = Group(delimitedList(table))
      userName = delimitedList(ident, ",", combine=True)
      userNameList = Group(delimitedList(userName))

      grantStmt = Forward()
      grantStmt << (grantToken + privilegeList.setResultsName("privileges") + 
		  onToken + tableList.setResultsName("tables") + 
		  toToken + userNameList.setResultsName("users") +
		  Optional(withToken + restOfLine.setResultsName("grantOption")))

      try:
	  tokens = grantStmt.parseString( string )
	  
      except ParseException, err:
	  return " "*err.loc + "^\n" + err.msg
      print  
      return tokens
    

  def AK_parse_drop(self, string):
      '''
      @autor mparavac
      @brief sql parsing of DROP command
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string 
      '''
      ident = Word( alphas, alphanums + "_$" ).setName("identifier")
      dropToken=Keyword("drop", caseless=True)
      objectToken=( Keyword("table", caseless=True) | Keyword("user", caseless=True) | Keyword("column", caseless=True) |\
                    Keyword("index", caseless=True) | Keyword("sequence", caseless=True) |\
                    Keyword("function", caseless=True) | Keyword("procedure", caseless=True) |\
                    Keyword("schema", caseless=True) | Keyword("trigger", caseless=True) | Keyword("role", caseless=True)
		  )
      onToken=Keyword("on", caseless=True)
      optionalToken1= Keyword("temporary", caseless=True)
      optionalToken2= Keyword("if", caseless=True) + Keyword("exists", caseless=True)
      optionalToken3= Keyword("cascade", caseless=True) | Keyword("restrict", caseless=True)
      objectName      = Upcase( delimitedList( ident, ",", combine=True ) )
      objectNameList  = Group( delimitedList( objectName ) )
      objectName2      = Upcase( delimitedList( ident, ",", combine=True ) )
      objectNameList2  = Group( delimitedList( objectName ) )
      optionalName = Upcase( delimitedList( ident, ",", combine=True ) )
      optionalNameList  = Group( delimitedList( objectName ) )
      sequenceName = Upcase( delimitedList( ident, ",", combine=True ) )
      sequenceNameList = Group( delimitedList( sequenceName ) )

      dropStmt=Forward()
      dropStmt << ( dropToken + Optional(optionalToken1.setResultsName("opcija1")) + objectToken.setResultsName("objekt") +\
                    Optional(optionalToken2.setResultsName("opcija2")) + objectNameList.setResultsName("ime_objekta") +\
                    Optional(optionalToken3.setResultsName("opcija3")) + Optional(onToken.setResultsName("onToken") +\
                    Optional(objectNameList2.setResultsName("ime_objekta2")))
		   
		  )
      try:
	tokens = dropStmt.parseString( string )
  
      except ParseException, err:
	return " "*err.loc + "^\n" + err.msg
      print
      return tokens

  def AK_alter_table(self, string):
      '''
      @autor Krunoslav Bilic
      @brief sql parsing of ALTER TABLE command (ref. http://www.w3schools.com/sql/sql_alter.asp )
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string 
      '''
      LPAR, RPAR, COMMA = map(Suppress,"(),")

      #keywords
      alterToken  = Keyword("alter",caseless=True).setResultsName("statement")
      addToken    = Keyword("add",caseless=True).setResultsName("operation")
      dropToken   = Keyword("drop",caseless=True).setResultsName("operation")
      tableToken  = Keyword("table",caseless=True)
      columnToken = Keyword("column",caseless=True)

      #definiranje gramatike
      slozeni = LPAR + ZeroOrMore(CharsNotIn(")")) + RPAR
      jednostavni = Word(alphas,alphanums+"_\"':-")

      #dohvacanje naziva tablice i stupca
      table_name = jednostavni.copy().setResultsName("table_name")       
      column_name = jednostavni.copy().setResultsName("column_name")
      #definiranje tipa podataka iz add naredbe
      field_def = OneOrMore(jednostavni | slozeni)
      field_list_def = delimitedList(field_def).setResultsName("data_type")

      #definiranje osnove upita te operacija dodavanja i uklanjanja stupca tablice
      alter_core = (alterToken + tableToken + table_name)
      adding = (addToken + column_name + field_list_def) 
      dropping = (dropToken + columnToken + column_name)

      #definiranje same naredbe
      alter_stmt = Forward()        
      alter_def =  (dropping) | (adding)
      alter_stmt << (alter_core + alter_def)

      try:
	tokens = alter_stmt.parseString( string )
      except ParseException, err:
	  return " "*err.loc + "^\n" + err.msg
      print  
      return tokens

  def AK_parse_createIndex(self, string):
      '''
      @autor Domagoj Tulicic
      @brief sql parsing of CREATE INDEX command
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string
      '''

      ident = Word( alphas, alphanums + "_$").setName("identifier")
      nazivIndexa = Word( alphas, alphanums + "_$").setName("identifier")
      nazivTablice = Word( alphas, alphanums + "_$").setName("identifier")
      createToken = Keyword("create", caseless=True)
      indexToken = Keyword("index", caseless = True)
      onToken = Keyword("on", caseless = True)
      usingToken = Keyword("using", caseless=True)
      column = delimitedList(ident, ",", combine=True)
      columnList = Group(delimitedList(column))
      lzagrada = Suppress("(")
      dzagrada = Suppress(")")

      createIndexStmt = Forward()
      createIndexStmt << (createToken + indexToken + nazivIndexa.setResultsName("IndexIme") + onToken +
                          nazivTablice.setResultsName("tablica") + lzagrada + columnList.setResultsName("stupci") + dzagrada +
                          usingToken + restOfLine.setResultsName("IndexVrsta"))
      try:
          tokens = createIndexStmt.parseString( string )
      except ParseException, err:
          return " "*err.loc + "^\n" + err.msg
      print
      return tokens

  def AK_create_sequence(self, string):

      '''
      @autor Domagoj Tulicic
      @brief sql parsing of CREATE SEQUENCE command
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string
      '''

      LPAR, RPAR, COMMA = map(Suppress,"(),")
      (CREATE, SEQUENCE, MINVALUE, MAXVALUE, START, WITH, INCREMENT, BY, CACHE ) =  map(CaselessKeyword, """CREATE,\
         SEQUENCE, MINVALUE, MAXVALUE, START, WITH, INCREMENT, BY, CACHE""".replace(",","").split())
      
      keyword = MatchFirst((CREATE, SEQUENCE, MINVALUE, MAXVALUE, START, WITH, INCREMENT, BY, CACHE))
      cycleToken = Keyword("cycle", caseless=True).setResultsName("cycle")

      identifier = ~keyword + Word(alphas, alphanums+"_")
      identifier2 = ~keyword + Word(nums)

      sequence_name = identifier.copy().setResultsName("sekvenca")
      min_value     = identifier2.copy().setResultsName("min_value")
      max_value     = identifier2.copy().setResultsName("max_value")
      start_with    = identifier2.copy().setResultsName("start_with")
      increment_by  = identifier2.copy().setResultsName("increment_by")
      cache_value   = identifier2.copy().setResultsName("cache")

      sequence_stmt = Forward()
      sequence_stmt << (CREATE + SEQUENCE + sequence_name + MINVALUE + min_value +\
			(Optional((MAXVALUE),default=MAXVALUE) +\
			Optional((max_value),default="999999999999999999999999999")) + START +  WITH +\
			start_with + INCREMENT + BY + increment_by + CACHE + cache_value + Optional(cycleToken)) 

      try:
          tokens = sequence_stmt.parseString(string)
      except ParseException, err:
	  return " "*err.loc + "^\n" + err.msg
      print
      return tokens

  def AK_parse_where(self, string):
      '''
      @author Kresimir Ivkovic
      @brief parser for select, delete and any function containing the "WHERE" clause (automatically detects statement type)
      @param string: sql query as string
      @return tokenized query
      '''      
      tokens = None
      
      #literals
      deleteLit=CaselessKeyword("DELETE")
      selectLit=CaselessKeyword("SELECT")
      fromLit=CaselessKeyword("FROM")
      usingLit=CaselessKeyword("USING")
      whereLit=CaselessKeyword("WHERE")
      lBracket=Suppress(Literal("("))
      rBracket=Suppress(Literal(")"))
      valuesLiteral=Suppress(CaselessLiteral("VALUES"))
      comma=Suppress(Literal(","))
      
      #identifier
      identifier=Word(alphas, alphanums + "_$")
      tableName=identifier
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
      columnName= tableName+'.'+identifier | identifier
      colAs = columnName + CaselessKeyword("AS") + identifier
      aggrfn = Group((CaselessKeyword("AVG") | CaselessKeyword("SUM") | CaselessKeyword("COUNT") | CaselessKeyword("MIN") | CaselessKeyword("MAX")) + lBracket + ('*' | columnName) + rBracket)
      columnNameSelect= Group(colAs) | aggrfn | (tableName+'.'+identifier) | identifier ### select stmt can contain aggr. functions in their att list, so a special columnName is needed for select
      columnNameList=Group(delimitedList(columnName))
      columnNameListSelect = Group(delimitedList(columnNameSelect) | '*') ### select can contain an asterisk for "all attributes" instead of att names
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
      sqlUnion=CaselessKeyword("UNION")+Optional(CaselessKeyword("ALL"))
      sqlIntersect=CaselessKeyword("INTERSECT")+Optional(CaselessKeyword("ALL"))
      sqlExcept=CaselessKeyword("EXCEPT")+Optional(CaselessKeyword("ALL"))
      predicate=Forward()
      predicate<<columnName+binrelop+value+ZeroOrMore((sqlOR | sqlAND | sqlIN | sqlBetween | sqlLike)+predicate)
      
      selectInner=Forward()
      expression=Forward()
      where=Forward()
      
      expression << Group(
            (columnName.setResultsName("expLval") + binrelop.setResultsName("whereOp") + columnName.setResultsName("expRval")) |
            (aggrfn.setResultsName("expLval") + binrelop.setResultsName("whereOp") + columnName.setResultsName("expRval")) |
            (columnName.setResultsName("expLval") + binrelop.setResultsName("whereOp") + value.setResultsName("expRval")) |
            (aggrfn.setResultsName("expLval") + binrelop.setResultsName("whereOp") + value.setResultsName("expRval")) |
            (columnName.setResultsName("expLval") + sqlIN.setResultsName("whereOp") + values.setResultsName("expRval")) |
            (columnName.setResultsName("expLval") + sqlIN.setResultsName("whereOp") + Group(selectInner.setResultsName("expRval")))
            ) + ZeroOrMore((sqlOR | sqlAND) + expression)

      where << whereLit.setResultsName("where")+expression.setResultsName("expression")
           
      select=Forward()
      join=Forward()
      distinct = CaselessKeyword("DISTINCT ON")+lBracket+columnNameList+rBracket
      limit = CaselessKeyword("LIMIT")+intNum.setResultsName("limitVal")
      offset = CaselessKeyword("OFFSET")+intNum.setResultsName("offsetVal")
      groupBy = CaselessKeyword("GROUP BY")+columnNameList.setResultsName("groupByCol")+Optional(CaselessKeyword("HAVING")+expression)
      orderBy = CaselessKeyword("ORDER BY")+columnNameList+Optional(CaselessKeyword("ASC")|CaselessKeyword("DESC"))

      joinStatement = Optional(
            CaselessKeyword("INNER")|
            CaselessKeyword("CROSS")|
            (
                  CaselessKeyword("LEFT")|
                  CaselessKeyword("RIGHT")|
                  CaselessKeyword("FULL")
            )+Optional(CaselessKeyword("OUTER"))
            )+CaselessKeyword("JOIN")+columnName.setResultsName("joinCol")+CaselessKeyword("ON")+expression

      join << joinStatement + ZeroOrMore(joinStatement)

      selectStatement=selectLit.setResultsName("commandName")+\
            Optional(Group(distinct.setResultsName("distinct")))+\
            columnNameListSelect.setResultsName("attributes")+\
            fromLit.setResultsName("from")+\
            tableName.setResultsName("tableName")+\
            Optional(Group(join.setResultsName("join")))+\
            Optional(where.setResultsName("condition"))+\
            Optional(Group(groupBy.setResultsName("group")))+\
            Optional(Group(orderBy.setResultsName("order")))+\
            Optional(Group(limit.setResultsName("limit")))+\
            Optional(Group(offset.setResultsName("offset")))

      select << selectStatement + ZeroOrMore((sqlUnion|sqlIntersect|sqlExcept) + select) + stringEnd
      selectInner << lBracket+selectStatement+rBracket + ZeroOrMore((sqlUnion|sqlIntersect|sqlExcept) + selectInner)

      deleteFrom=Forward()
      deleteFrom<<deleteLit.setResultsName("commandName")+\
        fromLit.setResultsName("from")+\
        tableName.setResultsName("tableName")+\
        Optional(usingLit.setResultsName("using")+tableName.setResultsName("usingTable"))+\
        Optional(where.setResultsName("condition"))+stringEnd
      
      sqlGrammar= select | deleteFrom
      
      try: 
          tokens = sqlGrammar.parseString(string)
      except ParseException, err:
          return "\n\t"+"Syntax error at char "+str(err.loc)+" "+err.msg+":\n"+\
            '\t"'+string+'"'+\
            '\n\t-'+''.join(['-' for i in range(0,err.loc)])+'^'+'\n'
      return tokens  

#--------------------------------------------testne funkcije--------------------------------------------#  


  def AK_parse_grant_test(self):
      '''
      @author Boris Kisic
      @brief testing of sql parsing command GRANT
      @return No return value
      '''
      print "\n---------------------------------GRANT test---------------------------------\n"
      commands = ["GRANT SELECT, INSERT, UPDATE, DELETE ON album, song TO Elvis, Jimmy WITH ADMIN OPTION",
		"grant update on table1, table2 to Ivica, pero22foi1",
		"Grant insert on drivers to Hamilton, Raikkonen, Alonso"]
	    
      for command in commands:
	token = test.AK_parse_grant(command)
	if isinstance(token, str):
	  print "Error: " + token
	  
	else:
	  print "tokens = ", token
	  print "tokens.privileges = ", token.privileges 
	  print "tokens.tables = ", token.tables
	  print "tokens.users =", token.users
	  print "tokens.grantOption =", token.grantOption
	  
  def AK_parse_drop_test(self):
     
     print "\n---------------------------------DROP test---------------------------------\n"
     commands=["DROP temporary table if exists tabela1231 cascade",
             "drop user matija, pero, jura",
             "Drop column kolona1, kolona_2",
             "drop index bla on tablica",
             "drop trigger bla on tablica2",
             "drop role test",
             "drop function if exists funcija1",
             "drop procedure if exists procedurea_df489f"]
             
     for command in commands:
       token = test.AK_parse_drop(command)
       if isinstance(token, str):
	 print "Error" + token
       else:
	  print "tokens = ", token
	  print "tokens.opcija1 = ", token.opcija1
	  print "tokens.objekt = ", token.objekt
	  print "tokens.ime_objekta = ", token.ime_objekta
	  print "tokens.opcija2 = ", token.opcija2
	  print "tokens.opcija3 = ", token.opcija3
	  print "tokens.onToken = ", token.onToken
	  print "tokens.ime_objekta2 = ", token.ime_objekta2	  

  def AK_alter_table_test(self):
    
     '''
     @author Krunoslav Bilic
     @brief testing of sql parsing command ALTER TABLE
     @return statement, operation, table_name, column_name, data_type[list]
     '''
     print "\n---------------------------------ALTER TABLE test---------------------------------\n"
     commands = ["alter table imena drop column srednje_ime",\
                 "alter table icecream add flavor varchar(20)",\
	         "alter table icecream add flavor",\
		 "alter table icecream drop flavor varchar(20)"]
	
     for command in commands:
        print "\n"+command
        token = test.AK_alter_table(command)
        if isinstance(token, str):
  	  print "\nError: " + token
        else:
	  print "tokens =      ", token
  	  print "statement =   ", token.statement
	  print "operation =   ", token.operation
	  print "table_name =  ", token.table_name
	  print "column_name = ", token.column_name
	  print "data_type =   ", token.data_type

  def AK_parse_createIndex_test(self):

     '''
     @author Domagoj Tulicic
     @brief testing of sql parsing command CREATE INDEX
     @return No return value
     '''
     print "\n---------------------------------CREATE INDEX test---------------------------------\n"
     commands = ["CREATE INDEX Pindex ON tablica ( stupac1, stupac2 ) USING Btree",\
                    "create index Pindex on tablica ( stupac1 ) USING Btree",\
                    "create index Pindex on tablica ( stupac1, stupac2 ) USING Hash"]

     for command in commands:
	  print "\n"+command
          token = test.AK_parse_createIndex(command)
          if isinstance(token, str):
          	print "Error: " + token
          else:
	        print "token = ", token
                print "IndexIme = ", token.IndexIme
                print "tablica = ", token.tablica
                print "stupci = ", token.stupci
                print "IndexVrsta = ", token.IndexVrsta

  def AK_create_sequence_test(self):

     '''
     @author Domagoj Tulicic
     @brief testing of sql parsing command CREATE SEQUENCE
     @return No return value
     '''
     print "\n---------------------------------CREATE SEQUENCE test---------------------------------\n"
     commands = ["create sequence sequenca minvalue 9 maxvalue 9999999 start with 1 increment by 2 cache 10 CYCLE",\
              "create sequence sequenca minvalue 9 maxvalue 9999999 start with 1 increment by 2 cache 10"]
    
     for command in commands:
         print "\n"+command
         token = test.AK_create_sequence(command)
         if isinstance(token, str):
             print "Error: " + token
	 else:
	     print "tokens = ", token
	     print "SekvencaIme = ", token.sekvenca
	     print "min value = ", token.min_value
             print "max value = ", token.max_value
             print "increment by = ", token.increment_by
             print "cache = ", token.cache
	     print "cycle = ", token.cycle


  def AK_parse_where_test(self):
      '''
      @author Kresimir Ivkovic
      @brief tests parsing of select and delete statements
      @return No return value
      '''
      
      query = ["select * from t1 inner join t2 on t1.at1 = t2.at1 left outer join t3 on t2.at2 = t3.at2 where t3.at3 > 0 and t2.at1 in (select count(*) from t3) order by t1.at1 desc limit 100 offset 10",
      "select at1, avg(at2) from t1 cross join t2 on t1.at1 > t2.at1 group by at1 having avg(at2) > 100 order by at1 asc union select count(*) from t3 where t3 in (select distinct on(a) a, b, c from t4)",
      "delete from t1 using t2 where t1.at1 = t2.at2",
      "delete from t1 where t1.at1 in (select * from t2 where at2 > 0 and at3 < 0 or at4 = 0) or t1.at2 in (select * from at3)"
      ]
      
      print "\n---------------------------------SELECT test---------------------------------\n"
      print query[0]
      print test.AK_parse_where(query[0])
      print '\n'
      print query[1]
      print test.AK_parse_where(query[1])      
      print "\n---------------------------------DELETE test---------------------------------\n"
      print query[2]
      print test.AK_parse_where(query[2])
      print '\n'
      print query[3]
      print test.AK_parse_where(query[3])


test = sql_tokenizer()

#testing grant statement
test.AK_parse_grant_test()

#testing drop statement
test.AK_parse_drop_test()

#testing alter statement
test.AK_alter_table_test()

#testing create sequence statement
test.AK_create_sequence_test()

#testing create index statement
test.AK_parse_createIndex_test()

#testing select and delete statements
test.AK_parse_where_test()
