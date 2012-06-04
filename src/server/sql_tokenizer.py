#@file sql_tokenizer.py - Parsing commands

from pyparsing import Word, alphas, alphanums, Keyword, delimitedList, Group, Forward, ParseException, Optional, restOfLine

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
	  
	
test_grant = sql_tokenizer()
test_grant.AK_parse_grant_test()
