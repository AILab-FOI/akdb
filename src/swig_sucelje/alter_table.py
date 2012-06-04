import sys
from pyparsing import *

#ALTER TABLE table_name ADD column_name data_type
#ALTER TABLE table_name DROP COLUMN column_name

LPAR, RPAR, COMMA = map(Suppress,"(),")
drop_stmt = Forward().setName("alter table")

#keywords
(ALTER, TABLE, ADD, DROP, COLUMN ) =  map(CaselessKeyword, """ALTER, TABLE, ADD, DROP, COLUMN""".replace(",","").split())

keyword = MatchFirst((ALTER, TABLE, ADD, DROP, COLUMN )) 

identifier = ~keyword + Word(alphas, alphanums+"_")

table_name = identifier.copy()
column_name = identifier.copy()
data_type = identifier.copy()


#single_source = ( Group(table_name("table") | table_name("table"))) 

drop_core = (ALTER + TABLE + table_name)
adding = (ADD + column_name + data_type) 
dropping = (DROP + COLUMN + column_name)

drop_stmt << (drop_core + Optional(adding | dropping))

tests = """\
    alter table ime1 drop column stupac add stupac tip """.splitlines()
for t in tests:
    print t
    try:
        print drop_stmt.parseString(t).dump()
    except ParseException, pe:
        print pe.msg
    print
    
