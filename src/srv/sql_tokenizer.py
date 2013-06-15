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
      @autor Domagoj Tulicic modified by Danko Sacer
      @brief sql parsing of CREATE SEQUENCE command
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string
      '''

      LPAR, RPAR, COMMA = map(Suppress,"(),")
      (CREATE, SEQUENCE, AS, START, WITH, INCREMENT, BY, MINVALUE, MAXVALUE,  CACHE ) =  map(CaselessKeyword, """CREATE, SEQUENCE, AS, START, WITH, INCREMENT, BY, MINVALUE, MAXVALUE, CACHE""".replace(",","").split())
      
      keyword = MatchFirst((CREATE, SEQUENCE, AS, START, WITH, INCREMENT, BY, MINVALUE, MAXVALUE, CACHE))
      cycleToken = Keyword("cycle", caseless=True).setResultsName("cycle")
      
      identifier = ~keyword + Word(alphas, alphanums+"_")
      identifier2 = ~keyword + Word(nums)

      sequence_name = identifier.copy().setResultsName("seq_name")
      as_value = identifier.copy().setResultsName("as_value")
      min_value     = identifier2.copy().setResultsName("min_value")
      max_value     = identifier2.copy().setResultsName("max_value")
      start_with    = identifier2.copy().setResultsName("start_with")
      increment_by  = identifier2.copy().setResultsName("increment_by")
      cache_value   = identifier2.copy().setResultsName("cache")
      	
      sequence_stmt = Forward()
      sequence_stmt << (CREATE + SEQUENCE + sequence_name +\
      	(Optional((AS),default=AS) + Optional((as_value),default="bigint")) +\
      	(Optional((START), default=START) + Optional((WITH),default=WITH) +\
	 Optional((start_with),default="no start")) +\
	(Optional((INCREMENT),default=INCREMENT) + Optional((BY),default=BY) +\
	 Optional((increment_by),default="1")) +\
      	(Optional((MINVALUE),default=MINVALUE) +\
      	 Optional((min_value),default="no minvalue")) +\
      	(Optional((MAXVALUE),default=MAXVALUE) +\
	 Optional((max_value),default="no maxvalue")) +\
	(Optional((CACHE),default=CACHE) +\
	 Optional((cache_value),default="15")) +\
	Optional((cycleToken),default="0")) 

      try:
          tokens = sequence_stmt.parseString(string)
      except ParseException, err:
          return " "*err.loc + "^\n" + err.msg
      print
      
      #definiranje min, max i start default vrijednosti na temelju tipa sequence
      if(tokens.as_value[0]=="smallint"):
      	if(tokens.min_value=="no minvalue"):
      		tokens.min_value = "-32768"
      		if(tokens.start_with=="no start"):
		      	tokens.start_with = tokens.min_value
		else:
			tokens.start_with = tokens.start_with[0]
      	else:
      		tokens.min_value = tokens.min_value[0]
      		if(tokens.start_with=="no start"):
		      	tokens.start_with = tokens.min_value[0]
		else:
			tokens.start_with = tokens.start_with[0]
      	if(tokens.max_value=="no maxvalue"):
      		tokens.max_value = "32767"
      	else:
      		tokens.max_value = tokens.max_value[0]
      
      elif(tokens.as_value[0]=="int"):
      	if(tokens.min_value=="no minvalue"):
      		tokens.min_value = "-2147483648"
      		if(tokens.start_with=="no start"):
		      	tokens.start_with = tokens.min_value
		else:
			tokens.start_with = tokens.start_with[0]
      	else:
      		tokens.min_value = tokens.min_value[0]
      		if(tokens.start_with=="no start"):
		      	tokens.start_with = tokens.min_value[0]
		else:
			tokens.start_with = tokens.start_with[0]
      	if(tokens.max_value=="no maxvalue"):
      		tokens.max_value = "2147483647"
      	else:
      		tokens.max_value = tokens.max_value[0]
      
      elif(tokens.as_value[0]=="bigint" or tokens.as_value=="bigint"):
      	if(tokens.min_value=="no minvalue"):
      		tokens.min_value = "-9223372036854775808"
      		if(tokens.start_with=="no start"):
		      	tokens.start_with = tokens.min_value
		else:
			tokens.start_with = tokens.start_with[0]
      	else:
      		tokens.min_value = tokens.min_value[0]
      		if(tokens.start_with=="no start"):
		      	tokens.start_with = tokens.min_value[0]
		else:
			tokens.start_with = tokens.start_with[0]
      	if(tokens.max_value=="no maxvalue"):
      		tokens.max_value = "9223372036854775807"
      	else:
      		tokens.max_value = tokens.max_value[0]
      
      elif(tokens.as_value[0]=="tinyint" or tokens.as_value[0]=="numeric" or tokens.as_value[0]=="decimal"):
      	if(tokens.min_value=="no minvalue"):
      		tokens.min_value = "0"
      		if(tokens.start_with=="no start"):
		      	tokens.start_with = tokens.min_value
		else:
			tokens.start_with = tokens.start_with[0]
      	else:
      		tokens.min_value = tokens.min_value[0]
      		if(tokens.start_with=="no start"):
		      	tokens.start_with = tokens.min_value[0]
		else:
			tokens.start_with = tokens.start_with[0]
      	if(tokens.max_value=="no maxvalue"):
      		tokens.max_value = "255"
      	else:
      		tokens.max_value = tokens.max_value[0]
      
      if(tokens.cache!="15"):
      	tokens.cache = tokens.cache[0]
      if(tokens.increment_by!="1"):
      	tokens.increment_by = tokens.increment_by[0]
      if(tokens.as_value!="bigint"):
      	tokens.as_value = tokens.as_value[0]
      if(tokens.cycle!="0"):
      	tokens.cycle="1"
      if(tokens.seq_name!=" "):
      	name = tokens.seq_name[0]
      	tokens.seq_name = name
      
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

  def AK_parse_create_user(self, string):
      '''
      @author Franjo Kovacic
      @brief sql parsing of CREATE USER command
      @param string sql command as string
      @return if command is successfully parsed returns list od tokens, else returns error message as string
      '''
 
      createUserToken = Keyword("CREATE USER", caseless=True)
      setPasswordToken = Keyword("WITH PASSWORD", caseless=True)
      createDBToken = Keyword("CREATEDB", caseless=True)
      createUsToken = Keyword("CREATEUSER", caseless=True)
      inGroupToken = Keyword("IN GROUP", caseless=True)
      validUntilToken = Keyword("VALID UNTIL", caseless=True)

      tokens = Word(alphanums+"_:.-")
      userName = tokens.copy().setResultsName("username")
      password = tokens.copy().setResultsName("password")
      groupName = tokens.setResultsName("groupName")
      validUntil = tokens.setResultsName("validUntil")

      constraints = ZeroOrMore(setPasswordToken + password.setResultsName("password") | createDBToken | createUsToken.setResultsName("createUser") | inGroupToken + groupName.setResultsName("groupName") | validUntilToken + validUntil.setResultsName("validUntil"))

      createUserStmt = createUserToken.setResultsName("commandName")+\
             userName.setResultsName("username")+\
             Optional(constraints)+\
             stringEnd

      try:
        tokens = createUserStmt.parseString(string)
      except ParseException, err:
        return " "*err.loc + "^\n" + err.msg
      print
      return tokens

  def AK_parse_create_table(self, string):
      '''
      @author Franjo Kovacic
      @brief sql parsing of CREATE TABLE command
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string
      '''
      
      createTableToken = Keyword("CREATE TABLE", caseless=True)
      notNullToken = Keyword("NOT NULL", caseless=True)
      uniqueToken = Keyword("UNIQUE", caseless=True)
      primaryKeyToken = Keyword("PRIMARY KEY", caseless=True)
      defaultToken = Keyword("DEFAULT", caseless=True)
      foreignKeyToken = Keyword("FOREIGN KEY", caseless=True)
      referencesToken = Keyword("REFERENCES", caseless=True)
      autoIncrementToken = Keyword("AUTO_INCREMENT", caseless=True)
      checkToken = Keyword("CHECK", caseless=True)
      lBracket=Suppress(Literal("("))
      rBracket=Suppress(Literal(")"))
      comma=Suppress(Literal(","))

      identifier=Word(alphas, alphanums + "_$")
      tableName = identifier
      columnName = identifier

      columnNameList=Group(delimitedList(columnName))
      columns=lBracket+columnNameList+rBracket

     
      #values
      E = CaselessLiteral("e")
      arithSign=Word("+-",exact=1)
      realNum=Combine(Optional(arithSign)+(Word(nums)+"."+Optional( Word(nums))|("."+Word(nums)))+Optional(E+Optional(arithSign)+Word(nums)))
      intNum=Combine(Optional(arithSign)+Word(nums)+Optional(E+Optional("+")+Word(nums)))
      value=quotedString | realNum | intNum
      valuesList=Group(delimitedList(value))
      values=lBracket+valuesList+rBracket


      #types
      dataSize=lBracket+intNum+rBracket
      floatType=CaselessKeyword("float")
      integerType=CaselessKeyword("int")
      varcharType=CaselessKeyword("varchar")+dataSize
      textType=CaselessKeyword("text")


      #predicate(limited)
      binrelop=oneOf(["<", "<=", ">=", ">", "=", "!="])
      sqlOR=CaselessKeyword("OR")
      sqlAND=CaselessKeyword("AND")
      predicate=Forward()
      predicate<<columnName+binrelop+value+ZeroOrMore((sqlOR | sqlAND)+predicate)

      #attribute constraint
      defaultConstraint = defaultToken + value
      foreignKeyConstraint = foreignKeyToken + referencesToken + tableName + columns
      checkConstraint = Group(CaselessKeyword("CHECK") + predicate)
      constraint = notNullToken | uniqueToken | primaryKeyToken | checkConstraint | autoIncrementToken
      constraints = ZeroOrMore(constraint)

      #constraint at the end 
      pkUnique=uniqueToken | primaryKeyToken
      pkUniqueEConstraint=pkUnique + columns
      foreignKeyEConstraint=foreignKeyToken+columnName+referencesToken+tableName+lBracket+columnName+rBracket
      checkEConstraint="ERROR"
      endConstraint=Group(CaselessKeyword("CONSTRAINT")+identifier+(pkUniqueEConstraint | checkEConstraint | foreignKeyEConstraint))

      endConstraints=Group(delimitedList(endConstraint))

      #attributes (create table)
      attributeType=floatType | integerType | varcharType | textType
      attributeDefinition=Group(identifier+attributeType+constraints)
      attributeDefinitionList=Group(delimitedList(attributeDefinition))
      attributes=attributeDefinitionList
 
      createTableStmt=createTableToken.setResultsName("commandName")+\
             tableName.setResultsName("tableName")+\
             lBracket+\
             attributes.setResultsName("attributes")+\
             Optional(comma+endConstraints).setResultsName("endConstraints")+\
             rBracket+stringEnd

      try:
        tokens = createTableStmt.parseString(string)
      except ParseException, err:
        return " "*err.loc + "^\n" + err.msg
      print
      return tokens
 

  def AK_parse_insert_into(self, string):
      '''
      @author Franjo Kovacic
      @brief sql parsing of INSERT INTO command
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string
      '''

      insertIntoToken = Keyword("INSERT INTO", caseless=True)
      valuesToken = Keyword("VALUES", caseless=True)
      lBracket=Suppress(Literal("("))
      rBracket=Suppress(Literal(")"))
      comma=Suppress(Literal(","))

      identifier=Word(alphas, alphanums + "_$")
      tableName=identifier

      columnName=identifier
      columnNameList=Group(delimitedList(columnName))
      columns=lBracket+columnNameList+rBracket

      #values
      E = CaselessLiteral("e")
      arithSign=Word("+-",exact=1)
      realNum=Combine(Optional(arithSign)+(Word(nums)+"."+Optional( Word(nums))|("."+Word(nums)))+Optional(E+Optional(arithSign)+Word(nums)))
      intNum=Combine(Optional(arithSign)+Word(nums)+Optional(E+Optional("+")+Word(nums)))
      value=quotedString | realNum | intNum
      valuesList=Group(delimitedList(value))
      values=lBracket+valuesList+rBracket

      insertStmt=insertIntoToken.setResultsName("commandName")+\
        tableName.setResultsName("tableName")+\
        Optional(columns).setResultsName("columns")+\
        valuesToken+\
        values.setResultsName("columnValues")+stringEnd

      try:
        tokens = insertStmt.parseString(string)
      except ParseException, err:
        return " "*err.loc + "^\n" + err.msg
      return tokens
      
  def AK_parse_trigger(self, string):
      '''
      @author Davorin Vukelic
      @brief sql parsing of TRIGGER command
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string 
      '''
  
      createToken = Keyword("create trigger", caseless=True)
      whenToken   = ( Keyword("after", caseless=True) | Keyword("before", caseless=True) )
      actionToken   = ( Keyword("delete", caseless=True) | Keyword("insert", caseless=True) | Keyword("update", caseless=True) )
      onToken   = Keyword("on", caseless=True)
      forToken =  Keyword("for", caseless=True) 
      eachToken = Keyword("each", caseless=True) 
      whatToken =  (Keyword("row", caseless=True) | Keyword("statement", caseless=True) )
      executeToken = Keyword("each", caseless=True)
      procedureToken = Keyword("execute procedure", caseless=True)
      orToken = Keyword("or", caseless=True)
      eventToken = ( Keyword("insert", caseless=True) | Keyword("update", caseless=True) | Keyword("delete", caseless=True))
      
      tokens =Word( alphas, alphanums + "_$")
      
      trigName = tokens.copy().setResultsName("name")
      event = tokens.copy().setResultsName("event")
      table = tokens.copy().setResultsName("tableName")
      function = tokens.copy().setResultsName("functionName")
      ident=Word(nums,alphas)
      param = delimitedList(ident, ",", combine=True)
      paramList = Group(delimitedList(param))
      lBracket = Suppress("(")
      rBracket = Suppress(")")

      #params
      E = CaselessLiteral("e")
      arithSign=Word("+-",exact=1)
      realNum=Combine(Optional(arithSign)+(Word(nums)+"."+Optional( Word(nums))|("."+Word(nums)))+Optional(E+Optional(arithSign)+Word(nums)))
      intNum=Combine(Optional(arithSign)+Word(nums)+Optional(E+Optional("+")+Word(nums)))
      value=quotedString | realNum | intNum
      valuesList=Group(delimitedList(value))
      


      triggerStmt = Forward()
      triggerStmt << (createToken+trigName+whenToken.setResultsName("whenOption")+\
                        eventToken.setResultsName("EventOption1")+Optional(orToken+\
                              eventToken.setResultsName("EventOption2")+Optional(orToken+\
                              eventToken.setResultsName("EventOption3")))+onToken+table+\
                              Optional(forToken+Optional(eachToken)+whatToken.setResultsName("whatOption"))+\
                              procedureToken+function+lBracket+Optional(valuesList.setResultsName("params"))+rBracket)

     
      try:
          tokens = triggerStmt.parseString( string )
          
      except ParseException, err:
          return " "*err.loc + "^\n" + err.msg
      print  
      return tokens


  def AK_alter_group(self, string):
      '''
      @autor Zvonimir Kapes
      @brief sql parsing of ALTER GROUP command
      @source http://www.postgresql.org/docs/9.1/static/sql-altergroup.html
      
        ALTER GROUP group_name ADD USER user_name [, ... ]
        ALTER GROUP group_name DROP USER user_name [, ... ]
        ALTER GROUP group_name RENAME TO new_name 

      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string 
      '''
      
      #keywords
      alterGroupToken   = Keyword("alter group",caseless=True).setResultsName("statement")
      addUserToken      = Keyword("add user",caseless=True).setResultsName("operation")
      dropUserToken     = Keyword("drop user",caseless=True).setResultsName("operation")
      renameGroupToken  = Keyword("rename to",caseless=True).setResultsName("operation")

      #values
      wrd = Word(alphanums + "_:.-")
      groupName = wrd.copy().setResultsName("groupname")
      userName = wrd.copy().setResultsName("username")
      groupNewName = wrd.copy().setResultsName("groupNewName")
      
      #list of usernames
      users = Group( delimitedList( userName , ",", combine=False)).setResultsName("users")
      
      #keyword + value
      addUserToken += users
      dropUserToken += users
      renameGroupToken += groupNewName
      
      alter_group_def = (addUserToken) | (dropUserToken) | (renameGroupToken)
      
      alterGroupStmt = alterGroupToken + groupName + alter_group_def + stringEnd

      try:
        tokens = alterGroupStmt.parseString(string)
      except ParseException, err:
          return " "*err.loc + "^\n" + err.msg
      print  
      return tokens
      
  def AK_alter_sequence(self, string):
      '''
      @autor Zvonimir Kapes
      @brief sql parsing of ALTER SEQUENCE command
      @source http://www.postgresql.org/docs/9.1/static/sql-altersequence.html
      
        ALTER SEQUENCE name [ INCREMENT [ BY ] increment ]
            [ MINVALUE minvalue | NO MINVALUE ] [ MAXVALUE maxvalue | NO MAXVALUE ]
            [ START [ WITH ] start ]
            [ RESTART [ [ WITH ] restart ] ]
            [ CACHE cache ] [ [ NO ] CYCLE ]
            [ OWNED BY { table.column | NONE } ]
        ALTER SEQUENCE name OWNER TO new_owner
        ALTER SEQUENCE name RENAME TO new_name      
      
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string 
      '''
      
      #keywords
      alterSeqToken = Keyword("alter sequence",caseless=True).setResultsName("statement")
      
      incrementToken= Keyword("increment", caseless=True).setResultsName("incOperation")
      minvalueToken = Keyword("minvalue", caseless=True).setResultsName("minvOperation")
      maxvalueToken = Keyword("maxvalue", caseless=True).setResultsName("maxvOperation")
      startToken    = Keyword("start", caseless=True).setResultsName("startOperation")
      restartToken  = Keyword("restart", caseless=True).setResultsName("restartOperation")
      cacheToken    = Keyword("cache", caseless=True).setResultsName("cacheOperation")
      renameToken   = Keyword("rename to", caseless=True).setResultsName("renameOperation")
      ownerToken    = Keyword("owner to", caseless=True).setResultsName("ownerOperation")
      #value keywords
      noMin         = Keyword("NO MINVALUE", caseless=True).setResultsName("minvalue")
      noMax         = Keyword("NO MAXVALUE", caseless=True).setResultsName("maxvalue")
      cycle         = Keyword("CYCLE", caseless=True).setResultsName("cycle")
      noCycle       = Keyword("NO CYCLE", caseless=True).setResultsName("cycle")
      
      #optional
      byToken   = Keyword("by", caseless=True)
      withToken = Keyword("with", caseless=True)

      #values
      identifier = Word(alphas, alphanums+"_")
      identifier2= Word(nums)
      sequence_name = identifier.copy().setResultsName("name")
      increment  = identifier2.copy().setResultsName("increment")
      minvalue   = identifier2.copy().setResultsName("minvalue")
      maxvalue   = identifier2.copy().setResultsName("maxvalue")
      start      = identifier2.copy().setResultsName("start")
      restart    = identifier2.copy().setResultsName("restart")
      cache      = identifier2.copy().setResultsName("cache")
      new_name   = identifier.copy().setResultsName("newName")
      new_owner  = identifier.copy().setResultsName("newOwner")         
      
      #keyword [optional] value
      incrementToken= incrementToken + Optional(byToken) + increment
      minvalueToken = (minvalueToken + minvalue) | (noMin)
      maxvalueToken = (maxvalueToken + maxvalue) | (noMax)
      startToken    = startToken + Optional(withToken) + start
      restartToken  = restartToken + Optional(Optional(withToken) + restart)
      cacheToken    = cacheToken + cache
      cycleToken    = (cycle) | (noCycle)
      renameToken   = renameToken + new_name
      ownerToken    = ownerToken + new_owner
      
      attribs = ZeroOrMore(incrementToken | minvalueToken | maxvalueToken | startToken | restartToken | cacheToken | cycleToken)
      
      alterSequenceStmt = alterSeqToken + sequence_name + Or(ownerToken | renameToken | attribs) + stringEnd     
      
      
      try:
        tokens = alterSequenceStmt.parseString(string)
      except ParseException, err:
          return " "*err.loc + "^\n" + err.msg
      
      s = set()
      print "TOKENSI : ",  tokens
      for x in tokens:
          if x in s: return "Error: redundant options"
          s.add(x)
      
      return tokens
      
  def AK_alter_index(self, string):
      '''
      @autor Zvonimir Kapes
      @brief sql parsing of ALTER INDEX command
      @source http://www.postgresql.org/docs/9.1/static/sql-alterindex.html
      
          ALTER INDEX name RENAME TO new_name
          ALTER INDEX name SET TABLESPACE tablespace_name
          ALTER INDEX name SET ( storage_parameter = value [, ... ] )
          ALTER INDEX name RESET ( storage_parameter [, ... ] )
          
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string 
      '''
      
      #keywords
      alterIndexToken   = Keyword("alter index",caseless=True).setResultsName("statement")
      renameToken       = Keyword("rename to",caseless=True).setResultsName("operation")
      setTablespaceToken= Keyword("set tablespace",caseless=True).setResultsName("operation")
      setToken          = Keyword("set",caseless=True).setResultsName("operation")
      resetToken        = Keyword("reset",caseless=True).setResultsName("operation")

      #values
      wrd = Word(alphanums + "_:.-")
      indexName         = wrd.copy().setResultsName("indexName")
      indexNewName         = wrd.copy().setResultsName("indexNewName")
      tableSpaceName    = wrd.copy().setResultsName("tableSpaceName")
      storageParameter  = wrd.copy().setResultsName("storageParameter")
      storageParameterValue = wrd.copy().setResultsName("storageParameterValue")
      
      #list of parameters
      storageParametersValues = Group(storageParameter+ Suppress("=") +storageParameterValue)
      storageParameters = Group( delimitedList( storageParametersValues , ",", combine=False)).setResultsName("storageParameters")
      
      #keyword + value
      renameToken += indexNewName
      setTablespaceToken += tableSpaceName
      setToken += Suppress("(") + storageParameters + Suppress(")")
      resetToken += storageParameters
      
      alter_index_def = (renameToken) | (setTablespaceToken) | (setToken) | (resetToken)
      
      alterIndexStmt = alterIndexToken + indexName + alter_index_def + stringEnd

      try:
        tokens = alterIndexStmt.parseString(string)
      except ParseException, err:
          return " "*err.loc + "^\n" + err.msg
      print  
      return tokens
      
  def AK_alter_user(self, string):
      '''
      @autor Zvonimir Kapes
      @brief sql parsing of ALTER USER command 
      @source http://www.postgresql.org/docs/8.4/static/sql-alteruser.html
      
        ALTER USER name [ [ WITH ] option [ ... ] ]
        where option can be:
              SUPERUSER | NOSUPERUSER
            | CREATEDB | NOCREATEDB
            | CREATEROLE | NOCREATEROLE
            | CREATEUSER | NOCREATEUSER
            | INHERIT | NOINHERIT
            | LOGIN | NOLOGIN
            | CONNECTION LIMIT connlimit
            | [ ENCRYPTED | UNENCRYPTED ] PASSWORD 'password'
            | VALID UNTIL 'timestamp' 

        ALTER USER name RENAME TO newname
        
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string 
      '''
      
      #keywords
      alterUserToken= Keyword("alter user",caseless=True).setResultsName("statement")
      renameToken   = Keyword("rename to",caseless=True).setResultsName("operation")
      withToken     = CaselessKeyword("with")
      #option keywords
      typeToken     = (CaselessKeyword("SUPERUSER") | CaselessKeyword("NOSUPERUSER")).setResultsName("typeOption")
      dbToken       = (CaselessKeyword("CREATEDB") | CaselessKeyword("NOCREATEDB")).setResultsName("dbOption")
      roleToken     = (CaselessKeyword("CREATEROLE") | CaselessKeyword("NOCREATEROLE")).setResultsName("roleOption")
      userToken     = (CaselessKeyword("CREATEUSER") | CaselessKeyword("NOCREATEUSER")).setResultsName("userOption")
      inheritToken  = (CaselessKeyword("INHERIT") | CaselessKeyword("NOINHERIT")).setResultsName("inheritOption")
      loginToken    = (CaselessKeyword("LOGIN") | CaselessKeyword("NOLOGIN")).setResultsName("loginOption")
      connectionLimitToken = CaselessKeyword("CONNECTION LIMIT").setResultsName("connectionLimitOption")
      encryptToken= (CaselessKeyword("ENCRYPTED") | CaselessKeyword("UNENCRYPTED")).setResultsName("encryptOption")
      passwordToken = Keyword("PASSWORD",caseless=True).setResultsName("passwordOption")
      timestampToken =CaselessKeyword("VALID UNTIL").setResultsName("timestampOption")
          
      #values
      wrd = Word(alphanums + "_:.-")
      num = Word(nums)
      username      = wrd.copy().setResultsName("username")
      userNewName   = wrd.copy().setResultsName("userNewName")
      connectionLimit = num.copy().setResultsName("connectionLimit")
      password      = wrd.copy().setResultsName("password")
      timestamp     = wrd.copy().setResultsName("timestamp")
      
      #keyword + value
      renameToken = renameToken + userNewName
      connectionLimitToken = connectionLimitToken + connectionLimit
      passwordToken = passwordToken + Suppress("'") + password + Suppress("'")
      timestampToken = timestampToken + Suppress("'") + timestamp + Suppress("'")
      encryptToken = Optional(encryptToken) + passwordToken
      
      
      options = ZeroOrMore(typeToken | dbToken | roleToken\
          | userToken | inheritToken | loginToken\
          | connectionLimitToken | encryptToken | timestampToken)
      
      alterUserStmt = alterUserToken + username + Optional(withToken) + Or(renameToken |\
          options.setResultsName("options")) + stringEnd

      try:
        tokens = alterUserStmt.parseString(string)
      except ParseException, err:
          return " "*err.loc + "^\n" + err.msg
      print  
      
      #noCheckValues - values that should not be checked for redundancy
      noCheckValues = [tokens.connectionLimit, tokens.password, tokens.timestamp]
    
      s = set()
      for x in tokens:
          if x in s:
              if x not in noCheckValues:
                  return "Error: redundant options! "
          s.add(x)
      
      return tokens
      
  def AK_create_group(self, string):
      '''
      @autor Zvonimir Kapes
      @brief sql parsing of CREATE GROUP command
      @source http://www.postgresql.org/docs/9.2/static/sql-creategroup.html
      
        CREATE GROUP name [ [ WITH ] option [ ... ] ]
        where option can be:
              SUPERUSER | NOSUPERUSER
            | CREATEDB | NOCREATEDB
            | CREATEROLE | NOCREATEROLE
            | CREATEUSER | NOCREATEUSER
            | INHERIT | NOINHERIT
            | LOGIN | NOLOGIN
            | [ ENCRYPTED | UNENCRYPTED ] PASSWORD 'password'
            | VALID UNTIL 'timestamp'
            | IN ROLE role_name [, ...]
            | IN GROUP role_name [, ...]
            | ROLE role_name [, ...]
            | ADMIN role_name [, ...]
            | USER role_name [, ...]
            | SYSID uid
            
      @param string sql command as string
      @return if command is successfully parsed returns list of tokens, else returns error message as string 
      '''
      
      #keywords
      createGroupToken  = Keyword("CREATE GROUP",caseless=True).setResultsName("statement")
      withToken     = CaselessKeyword("with")
      #option keywords
      userTypeToken     = (CaselessKeyword("SUPERUSER") | CaselessKeyword("NOSUPERUSER")).setResultsName("userTypeOption")
      dbToken           = (CaselessKeyword("CREATEDB") | CaselessKeyword("NOCREATEDB")).setResultsName("dbOption")
      createRoleToken   = (CaselessKeyword("CREATEROLE") | CaselessKeyword("NOCREATEROLE")).setResultsName("createRoleOption")
      createUserToken   = (CaselessKeyword("CREATEUSER") | CaselessKeyword("NOCREATEUSER")).setResultsName("createUserOption")
      inheritToken      = (CaselessKeyword("INHERIT") | CaselessKeyword("NOINHERIT")).setResultsName("inheritOption")
      loginToken        = (CaselessKeyword("LOGIN") | CaselessKeyword("NOLOGIN")).setResultsName("loginOption")
      encryptToken      = (CaselessKeyword("ENCRYPTED") | CaselessKeyword("UNENCRYPTED")).setResultsName("encryptOption")
      passwordToken     = CaselessKeyword("PASSWORD").setResultsName("passwordOption")
      timestampToken    = CaselessKeyword("VALID UNTIL").setResultsName("timestampOption")
      inRoleToken   = CaselessKeyword("IN ROLE").setResultsName("inRoleOption")
      inGroupToken  = CaselessKeyword("IN GROUP").setResultsName("inGroupOption")
      roleToken     = CaselessKeyword("ROLE").setResultsName("roleOption")
      adminToken    = CaselessKeyword("ADMIN").setResultsName("adminOption")
      userToken     = CaselessKeyword("USER").setResultsName("userOption")
      sysidToken    = CaselessKeyword("SYSID").setResultsName("sysidOption")
          
      #values
      wrd = Word(alphanums + "_:.-")
      groupname     = wrd.copy().setResultsName("groupname")
      password      = wrd.copy().setResultsName("password")
      timestamp     = wrd.copy().setResultsName("timestamp")
      inRole        = wrd.copy().setResultsName("inRole")
      inGroup       = wrd.copy().setResultsName("inGroup")
      role          = wrd.copy().setResultsName("role")
      admin         = wrd.copy().setResultsName("admin")
      user          = wrd.copy().setResultsName("user")
      sysid         = wrd.copy().setResultsName("sysid")
      
      #multiple values
      inRoles  = Group( delimitedList( inRole , ",", combine=False)).setResultsName("inRoles")
      inGroups = Group( delimitedList( inGroup , ",", combine=False)).setResultsName("inGroups")
      roles    = Group( delimitedList( role , ",", combine=False)).setResultsName("roles")
      admins   = Group( delimitedList( admin , ",", combine=False)).setResultsName("admins")
      users    = Group( delimitedList( user , ",", combine=False)).setResultsName("users")
      
      #keyword + value
      passwordToken = passwordToken + Suppress("'") + password + Suppress("'")
      timestampToken= timestampToken + Suppress("'") + timestamp + Suppress("'")
      encryptToken  = Optional(encryptToken) + passwordToken
      inRoleToken   = inRoleToken + inRoles
      inGroupToken  = inGroupToken + inGroups
      roleToken     = roleToken + roles
      adminToken    = adminToken + admins
      userToken     = userToken + users
      sysidToken    = sysidToken + sysid

      options = ZeroOrMore(userTypeToken | dbToken | createRoleToken\
          | createUserToken | inheritToken | loginToken\
          | encryptToken | timestampToken | inRoleToken\
          | inGroupToken | roleToken\
          | adminToken | userToken | sysidToken)
      
      createGroupStmt = createGroupToken + groupname + Optional(withToken) + options.setResultsName("options") + stringEnd

      try:
        tokens = createGroupStmt.parseString(string)
      except ParseException, err:
          return " "*err.loc + "^\n" + err.msg
      print  
      
      #noCheckValues - values that should not be checked for redundancy
      noCheckValues = [tokens.password, tokens.timestamp]
    
      s = set()
      for x in tokens:
          if x in s:
              if x not in noCheckValues:
                  return "Error: redundant options! "
          s.add(x)
      
      return tokens




#--------------------------------------------testne funkcije--------------------------------------------#  


  def Ak_alter_group_test(self):
      '''
      @author Zvonimir Kapes
      @brief testing of sql parsing command ALTER GROUP
      @return No return value
      '''
      print "\n---------------------------------ALTER GROUP test---------------------------------\n"
      commands = ["ALTER GROUP grupa_rnm RENAME TO novoime",
                  "ALTER GROUP grupa_rnm RENAME TO",
                  "ALTER GROUP grupa_rnm RENAME TO novoime, drugoime",
                  "ALTER GROUP grupa_rnm RENAME TO novoime ssdw",
                "ALTER GROUP grupa_addus ADD USER pr_vi , dr.ugi, _treci",
                "ALTER GROUP grupa_addus ADD USER  , ",
                "ALTER GROUP grupa_addus ADD USER prvi drugi treci, chetrti",
                "ALTER GROUP grupa_addus DROP USER jedini,f",
                "ALTER GROUP grupa_drpus DROP USER prva, druga treci"]
            
      for command in commands:
        token = test.AK_alter_group(command)
        print "\n", command
        if isinstance(token, str):
          print "Error:"
          print token
          
        else:
          print "tokens = ", token
          print "tokens.statement = ", token.statement
          print "tokens.operation = ", token.operation
          print "tokens.groupname = ", token.groupname
          print "tokens.grouprename =", token.groupNewName
          print "tokens.userList = ", token.users
        
  def Ak_alter_sequence_test(self):
      '''
      @author Zvonimir Kapes
      @brief testing of sql parsing command ALTER SEQUENCE
      @return No return value
      '''
      print "\n---------------------------------ALTER SEQUENCE test---------------------------------\n"
      commands = ["ALTER SEQUENCE serial RESTART WITH 105",
                "ALTER sequencE sek_ve1nca INCREMENT 2 MAXVALUE 232 No minvalue START WITH 55 RESTART 16 CACHE 8",
                "ALTER sequencE sek_ve1nca INCREMENT 2 No minvalue NO MAXVALUE RESTART CACHE 8 no CYCLE start 343 RESTART",
                "ALTER SEQUENCE sekv Owner TO new_owner rename to lalal",
                "ALTER SEQUENCE sekv rename TO new_name"]
            
      for command in commands:
        token = test.AK_alter_sequence(command)
        print "\n", command
        if isinstance(token, str):
          print "Error:"
          print token
          
        else:
          print "tokens = ", token
          print "tokens.statement = ", token.statement
          print "tokens.name = ", token.name
          print "tokens.increment = ", token.increment
          print "tokens.minvalue = ", token.minvalue
          print "tokens.maxvalue = ", token.maxvalue
          print "tokens.start = ", token.start
          print "tokens.restart = ", token.restart
          print "tokens.cache = ", token.cache
          print "tokens.cycle = ", token.cycle
          print "tokens.newName = ", token.newName
          print "tokens.newOwner = ", token.newOwner
          
  def Ak_alter_index_test(self):
      '''
      @author Zvonimir Kapes
      @brief testing of sql parsing command ALTER INDEX
      @return No return value
      '''
      print "\n---------------------------------ALTER INDEX test---------------------------------\n"
      commands = ["ALTER INDEX distributors SET TABLESPACE fasttablespace",
                  "ALTER INDEX distributors RENAME TO noviNaziv",
                  "ALTER INDEX distributors SET (fillfactor = 75, opcija2=199)"]
            
      for command in commands:
        token = test.AK_alter_index(command)
        print "\n", command
        if isinstance(token, str):
          print "Error:"
          print token
          
        else:
          print "tokens = ", token
          print "tokens.statement = ", token.statement
          print "tokens.operation = ", token.operation
          print "tokens.indexNewName = ", token.indexNewName
          print "tokens.tableSpaceName =", token.tableSpaceName
          print "tokens.storageParameters = ", token.storageParameters
          
  def Ak_alter_user_test(self):
      '''
      @author Zvonimir Kapes
      @brief testing of sql parsing command ALTER USER
      @return No return value
      '''
      print "\n---------------------------------ALTER USER test---------------------------------\n"
      commands = ["ALTER USER username SUPERUSER CREATEDB password 'lozinka' Superuser CONNECTION limit 353 INHERIT valid until '12-06-2032'",
                  "ALTER USER korisnik nosuperuser login connection limit 11 password 'PASS234'",
                  "ALTER USER staroIme RENAME TO novoIme"]
            
      for command in commands:
        token = test.AK_alter_user(command)
        print "\n", command
        if isinstance(token, str):
          print "Error:"
          print token
          
        else:
          print "tokens = ", token
          print "tokens.username = ", token.username
          print "tokens.password = ", token.password
          print "tokens.options = ", token.options
          print "tokens.type = ", token.typeOption
          print "tokens.connectionLimit = ", token.connectionLimit
          
  def Ak_create_group_test(self):
      '''
      @author Zvonimir Kapes
      @brief testing of sql parsing command CREATE GROUP
      @return No return value
      '''
      print "\n---------------------------------CREATE GROUP test---------------------------------\n"
      commands = ["CREATE GROUP miriam valid until '12-06-2032' LOGIN PASSWORD 'jw8s0F4' VALID UNTIL '2005-01-01'",
                  "CREATE GROUP admin WITH user kor1, kor2 CREATEROLE CREATEDB SUPERUSER",
                  "CREATE GROUP grupa WITH CREATEDB CREATEROLE user kor1, kor2, kor3 valid until '12-06-2032'"]
            
      for command in commands:
        token = test.AK_create_group(command)
        print "\n", command
        if isinstance(token, str):
          print "Error:"
          print token
          
        else:
          print "tokens = ", token
          print "tokens.groupname = ", token.groupname
          print "tokens.dbOption = ", token.dbOption
          print "tokens.options = ", token.options
          print "tokens.users = ", token.users
          print "tokens.timestamp = ", token.timestamp

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
          print "Error:"
          print command
          print token
          
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
         print "Error:"
         print command
         print token
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
          print "Error:"
          print command
          print token
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
     commands = ["create sequence sequenca start with 1 increment by 2 minvalue 9 maxvalue 9999999  cache 10 CYCLE",\
              "create sequence sequenca start with 1 increment by 2 minvalue 9 maxvalue 9999999  cache 10"]
    
     for command in commands:
         print "\n"+command
         token = test.AK_create_sequence(command)
         if isinstance(token, str):
             print "Error:"
             print command
             print token
         else:
             print "tokens = ", token
             print "SekvencaIme = ", token.seq_name
             print "min value = ", token.min_value
             print "max value = ", token.max_value
             print "increment by = ", token.increment_by
             print "cache = ", token.cache
             print "cycle = ", token.cycle
             print "'AS' definition: ", token.as_value
             print "'Start with' value: ", token.start_with
     '''
     @author Danko Sacer
     @brief additional testing of sql parsing command CREATE SEQUENCE
     @return No return value
     '''
     print "\n********CREATE SEQUENCE test by Danko Sacer***********"
     
     sql_1 = "create sequence brojac_1 as smallint start with 10 increment by 1 minvalue 5"
     
     print "\n"+sql_1
     token = test.AK_create_sequence(sql_1)
     if isinstance(token, str):
     	print "Error: "
     	print sql_1
	print token
     else:
	print "Tokens: ", token
	print "\nSequence name: ", token.seq_name
	print "'AS' definition: ", token.as_value
	print "'Start with' value: ", token.start_with
	print "'Increment by' value: ", token.increment_by
	print "'MinValue' value: ", token.min_value
	print "'MaxValue' value: ", token.max_value
	print "'Cache' value: ", token.cache
	print "'Cycle' value: ", token.cycle
	     
     sql_2 = "create sequence brojac_2"
     
     print "\n"+sql_2
     token = test.AK_create_sequence(sql_2)
     if isinstance(token, str):
     	print "Error:"
     	print sql_2
     	print token
     else:
     	print "Tokens: ", token
     	print "\nSequence name: ", token.seq_name
     	print "'AS' definition: ", token.as_value
     	print "'Start with' value: ", token.start_with
     	print "'Increment by' value: ", token.increment_by
      	print "'MinValue' value: ", token.min_value
     	print "'MaxValue' value: ", token.max_value
     	print "'Cache' value: ", token.cache
      	print "'Cycle' value: ", token.cycle


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

  def AK_parse_create_user_test(self):
      '''
      @author Franjo Kovacic
      @brief tests parsing of CREATE USER statement
      @return No return value
      '''
      print "\n---------------------------------CREATE USER test---------------------------------\n"
      commands = ["CREATE USER test1 WITH PASSWORD tt", "CREATE USER user WITH PASSWORD 1pass1", "CREATE USER ub1 IN GROUP grupa", "CREATE USER mmw WITH PASSWORD pas1mac IN GROUP anim CREATEDB VALID UNTIL 2013-22-02"]
            
      for command in commands:
        token = test.AK_parse_create_user(command)
        if isinstance(token, str):
          print "Error:"
          print command
          print token
        else:
          print "tokens = ", token.dump()

  def AK_parse_create_table_test(self):
      '''
      @author Franjo Kovacic
      @brief tests parsing of CREATE TABLE statement
      @return No return value
      '''
      print "\n---------------------------------CREATE TABLE test---------------------------------\n"
      commands = ["CREATE TABLE tablica (var1 INT NOT NULL, var2 INT PRIMARY KEY)", "CREATE TABLE tabla1 (v2 INT, v4 TEXT, v11 INT AUTO_INCREMENT)"]
            
      for command in commands:
        token = test.AK_parse_create_table(command)
        if isinstance(token, str):
          print "Error:"
          print command
          print token
        else:
          print "tokens = ", token.dump()
          

  def AK_parse_insert_into_test(self):
      '''
      @author Franjo Kovacic
      @brief tests parsing of INSERT INTO statement
      @return No return value
      '''
      print "\n---------------------------------INSERT INTO test---------------------------------\n"
      commands = ["INSERT INTO tablica(vr1, v2, ttt3) VALUES ('a1', 'ss2', 'a2')", "INSERT INTO tablica1 VALUES (11, 'kk2', 'j2')"]
            
      for command in commands:
        token = test.AK_parse_insert_into(command)
        if isinstance(token, str):
          print "Error:"
          print command
          print token
          
        else:
          print "tokens = ", token.dump()


  def Ak_create_trigger_test(self):
      '''
      @author Davorin Vukelic
      @brief testing of sql parsing command Ak_create_trigger_test
      @return No return value
      '''
      print "\n---------------------------------TRIGGER test---------------------------------\n"

      commands=["CREATE TRIGGER prihvat_veze AFTER INSERT ON veza FOR ROW EXECUTE PROCEDURE veza_prihvacena()",\
                  "CREATE TRIGGER prihvat_veze BEFORE  DELETE OR INSERT ON veza EXECUTE PROCEDURE veza_prihvacena()",\
                  "CREATE TRIGGER prihvat_veze BEFORE DELETE OR INSERT ON veza FOR EACH STATEMENT EXECUTE PROCEDURE veza_prihvacena(2,3,'data')",\
                  "CREATE TRIGGER prihvat_veze AFTER DELETE OR INSERT OR UPDATE ON veza FOR EACH STATEMENT EXECUTE PROCEDURE veza_prihvacena(2,10.5,'data')"]
      
      
      for command in commands:
            token = test.AK_parse_trigger(command)
            if isinstance(token, str):
                  print "Error: " + token

            else:
                  print "tokens = ",token
                  print "tokens.name = ",token.name
                  print "tokens.whenOption = ",token.whenOption
                  print "tokens.EventOption1 = ",token.EventOption1
                  print "tokens.EventOption2 = ",token.EventOption2
                  print "tokens.EventOption3 = ",token.EventOption3
                  print "tokens.tableName = ",token.tableName
                  print "tokens.whatOption = ",token.whatOption
                  print "tokens.functionName = ",token.functionName
                  print "tokens.paramList = ",token.params



test = sql_tokenizer()
'''
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

#testing create user statement
test.AK_parse_create_user_test()

#testing create table statement
test.AK_parse_create_table_test()

#testing insert into statement
test.AK_parse_insert_into_test()

#testing create trigger statement
test.Ak_create_trigger_test()
'''
'''
#testing create sequence statement
test.AK_create_sequence_test()

#testing alter group statement
test.Ak_alter_group_test()

#testing alter sequence statement
test.Ak_alter_sequence_test()

#testing alter index statement
test.Ak_alter_index_test()

#testing alter user statement
test.Ak_alter_user_test()

#testing create group statement
test.Ak_create_group_test()
'''
