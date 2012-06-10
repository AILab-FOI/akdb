#@file sql_tokenizer.py - Parsing commands

from pyparsing import Word, alphas, alphanums, Keyword, delimitedList, Group, Forward, ParseException, Optional, restOfLine, Upcase

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
      objectToken=( Keyword("table", caseless=True) | Keyword("user", caseless=True) | Keyword("column", caseless=True) | Keyword("index", caseless=True) |
		  Keyword("sequence", caseless=True) | Keyword("function", caseless=True) | Keyword("procedure", caseless=True) | Keyword("schema", caseless=True) | Keyword("trigger", caseless=True) | Keyword("role", caseless=True)
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
      dropStmt << ( dropToken + Optional(optionalToken1.setResultsName("opcija1")) + objectToken.setResultsName("objekt") + Optional(optionalToken2.setResultsName("opcija2")) +
		   objectNameList.setResultsName("ime_objekta") + Optional(optionalToken3.setResultsName("opcija3")) + Optional(onToken.setResultsName("onToken") + Optional(objectNameList2.setResultsName("ime_objekta2")))
		   
		  )
      try:
	tokens = dropStmt.parseString( string )
  
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



#----------------------testne funkcije---------------#  


  def AK_parse_grant_test(self):
      '''
      @author Boris Kisic
      @brief testing of sql parsing command GRANT
      @return No return value
      '''
      commands = ["GRANT SELECT, INSERT, UPDATE, DELETE ON album, song TO Elvis, Jimmy WITH ADMIN OPTION",
		"grant update on table1, table2 to Ivica, pero22foi1",
		"Grant insert on drivers to Hamilton, Raikkonen, Alonso"]
	    
      for command in commands:
	token = test_grant.AK_parse_grant(command)
	if isinstance(token, str):
	  print "Error: " + token
	  
	else:
	  print "tokens = ", token
	  print "tokens.privileges = ", token.privileges 
	  print "tokens.tables = ", token.tables
	  print "tokens.users =", token.users
	  print "tokens.grantOption =", token.grantOption
	  
  def AK_parse_drop_test(self):
     
     commands=["DROP temporary table if exists tabela1231 cascade",
             "drop user matija, pero, jura",
             "Drop column kolona1, kolona_2",
             "drop index bla on tablica",
             "drop trigger bla on tablica2",
             "drop role test",
             "drop function if exists funcija1",
             "drop procedure if exists procedurea_df489f"]
             
     for command in commands:
       token = test_drop.AK_parse_drop(command)
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

test_grant = sql_tokenizer()
test_grant.AK_parse_grant_test()
test_drop = sql_tokenizer()
test_drop.AK_parse_drop_test()

def AK_parse_createIndex_test(self):

       '''
      @author Domagoj Tulicic
      @brief testing of sql parsing command CREATE INDEX
      @return No return value
      '''

        commands = ["CREATE INDEX Pindex ON tablica ( stupac1, stupac2 ) USING Btree",
                    "create index Pindex on tablica ( stupac1 ) USING Btree",
                    "create index Pindex on tablica ( stupac1, stupac2 ) USING Hash"]

        for command in commands:
            token = test_createIndex.AK_parse_createIndex(command)
            if isinstance(token, str):
                print "Error: " + token

            else:
                print "token = ", token
                print "token = ", token.IndexIme
                print "token = ", token.tablica
                print "token = ", token.stupci
                print "token = ", token.IndexVrsta



test_createIndex = sql_tokenizer()
test_createIndex.AK_parse_createIndex_test()
