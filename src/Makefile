# KalashnikovDB MakeFile

CC = gcc
DEBUG = -g
CFLAGS = $(DEBUG) -c -Wall

DISKTARGETS = dbman.o 
MEMORYTARGETS = memoman.o
FILETARGETS = files.o fileio.o filesearch.o filesort.o index.o btree.o hash.o bitmap.o table.o
RELOPTARGETS = difference.o intersect.o nat_join.o projection.o rename.o selection.o union.o aggregation.o product.o theta_join.o
OPTITARGETS = rel_eq_projection.o rel_eq_selection.o rel_eq_assoc.o rel_eq_comut.o query_optimization.o
CONSTRAINTTARGETS = reference.o between.o nnull.o id.o expression_check.o
OTHERTARGETS = trigger.o test.o debug.o

OUTDIR = ../bin

all: kalashnikov-db 

kalashnikov-db: auxiliary.o $(OTHERTARGETS) $(CONSTRAINTTARGETS) $(OPTITARGETS) $(RELOPTARGETS) $(DISKTARGETS) $(MEMORYTARGETS) $(FILETARGETS) main.o
	$(CC) main.o $(OTHERTARGETS) $(CONSTRAINTTARGETS) $(OPTITARGETS) $(RELOPTARGETS) $(FILETARGETS) $(MEMORYTARGETS) $(DISKTARGETS) auxiliary.o -o $(OUTDIR)/akdb

clean:
	rm -rf *~ *.o ../bin/akdb ../doc/* ../bin/kalashnikov.db

doc:
	doxygen doxygen.config
	cd ../doc/latex && make
	cp ../doc/latex/refman.pdf ../doc/
