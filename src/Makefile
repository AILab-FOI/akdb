# KalashnikovDB MakeFile

CC = gcc -pthread -w
DEBUG = -g
CFLAGS = $(DEBUG) -c -Wall 

DISKTARGETS = dbman.o 
MEMORYTARGETS = memoman.o
FILETARGETS = files.o fileio.o filesearch.o filesort.o index.o btree.o hash.o bitmap.o table.o
RELOPTARGETS = difference.o intersect.o nat_join.o projection.o rename.o selection.o union.o aggregation.o product.o theta_join.o transaction.o
OPTITARGETS = rel_eq_projection.o rel_eq_selection.o rel_eq_assoc.o rel_eq_comut.o query_optimization.o
CONSTRAINTTARGETS = reference.o between.o nnull.o id.o expression_check.o check_constraint.o unique.o
OTHERTARGETS = trigger.o test.o debug.o archive_log.o command.o ini_parser/dictionary.o ini_parser/iniparser.o privileges.o function.o sequence.o redo_log.o drop.o view.o observable.o

OUTDIR = ../bin

all: kalashnikov-db

kalashnikov-db: auxiliary.o $(OTHERTARGETS) $(CONSTRAINTTARGETS) $(OPTITARGETS) $(RELOPTARGETS) $(DISKTARGETS) $(MEMORYTARGETS) $(FILETARGETS) main.o
	$(CC) main.o $(OTHERTARGETS) $(CONSTRAINTTARGETS) $(OPTITARGETS) $(RELOPTARGETS) $(FILETARGETS) $(MEMORYTARGETS) $(DISKTARGETS) auxiliary.o -o $(OUTDIR)/akdb

clean:
	rm -rf *~ *.o iniparser/*.o ../bin/akdb ../bin/log.log ../doc/* ../bin/kalashnikov.db swig_interface/build swig_interface/*.pyc swig_interface/*.so swig_interface/*~ swig_interface/kalashnikovDB_wrap.c

doc:
	doxygen doxygen.config
	cd ../doc/latex && make
	cp ../doc/latex/refman.pdf ../doc/

swig:
	cd swig_interface && swig -python kalashnikovDB.i && python setup.py build_ext --inplace && cd ..
