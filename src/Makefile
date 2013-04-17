# KalashnikovDB MakeFile

CC = gcc -pthread -w
DEBUG = -g
CFLAGS = $(DEBUG) -c -Wall 

DISKTARGETS = dm/dbman.o 
MEMORYTARGETS = mm/memoman.o
FILETARGETS = file/files.o file/fileio.o file/filesearch.o file/filesort.o file/idx/index.o file/idx/btree.o file/idx/hash.o file/idx/bitmap.o file/table.o
RELOPTARGETS = rel/difference.o rel/intersect.o rel/nat_join.o rel/projection.o sql/rename.o rel/selection.o rel/union.o rel/aggregation.o rel/product.o rel/theta_join.o trans/transaction.o
OPTITARGETS = opti/rel_eq_projection.o opti/rel_eq_selection.o opti/rel_eq_assoc.o opti/rel_eq_comut.o opti/query_optimization.o
CONSTRAINTTARGETS = sql/cs/reference.o sql/cs/between.o sql/cs/nnull.o file/id.o rel/expression_check.o sql/cs/check_constraint.o sql/cs/unique.o
OTHERTARGETS = sql/trigger.o file/test.o aux/debug.o rec/archive_log.o sql/command.o aux/dictionary.o aux/iniparser.o sql/privileges.o sql/function.o rel/sequence.o rec/redo_log.o sql/drop.o sql/view.o aux/observable.o

OUTDIR = ../bin

all: kalashnikov-db

kalashnikov-db: aux/auxiliary.o $(OTHERTARGETS) $(CONSTRAINTTARGETS) $(OPTITARGETS) $(RELOPTARGETS) $(DISKTARGETS) $(MEMORYTARGETS) $(FILETARGETS) main.o
	$(CC) main.o $(OTHERTARGETS) $(CONSTRAINTTARGETS) $(OPTITARGETS) $(RELOPTARGETS) $(FILETARGETS) $(MEMORYTARGETS) $(DISKTARGETS) aux/auxiliary.o -o $(OUTDIR)/akdb

clean:
	rm -rf *~ *.o aux/*.o dm/*.o mm/*.o file/*.o file/idx/*.o rec/*.o sql/cs/*.o sql/*.o opti/*.o rel/*.o ../bin/akdb ../bin/log.log ../doc/* ../bin/kalashnikov.db swig/build swig/*.pyc swig/*.so swig/*~ swig/kalashnikovDB_wrap.c

doc:
	doxygen doxygen.config
	cd ../doc/latex && make
	cp ../doc/latex/refman.pdf ../doc/

swig:
	cd swig && swig -python kalashnikovDB.i && python setup.py build_ext --inplace && cd ..
