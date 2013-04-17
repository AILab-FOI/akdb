#@file sql_parser_test.py test cases for sql parser

from sql_parser import *

commands=["INSERt INO (ime2,nekiBroj) Studenti_2 values ('miro', 5.22)",
          "INsert INtO (ime2,nekiBroj) Studenti values ('miro', 5.22)",
          "INsert INtO Stud34_enti values ('miro', 5.22, 778)",
          "INSERT INTO tablica values(2,3) i jos nest",
          "CREATE TABLE nekaTablica",
          "CREATE TABLE nekaTablica (atribut1 float "+\
          "PRIMARY KEY UNIQUE, atribut2 varchar(2) CHECK atribut2<25 And atribut2>10, CONSTRAINT k1 Unique(atribut1), CONSTRAINT fk FOREIGN KEY(atribut1) REFERENCES drugaTablica(tr))",
          "CREATE TABLE error (atribut bloat)",
          "CREATE TABLE er2 (atr integer, atr float) PRIMARY KEY"
          ]

for command in commands:
    parsed=AKparseSql(command)
    if not isinstance(parsed, str):
        print parsed.commandName
    else:
        print "error"
    print command,"->",parsed, "\n"


