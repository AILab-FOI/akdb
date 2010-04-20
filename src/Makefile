# KalashnikovDB MakeFile

CC = gcc
DEBUG = -g
CFLAGS = $(DEBUG) -c -Wall

DISKTARGETS = dbman.o 
MEMORYTARGETS = memoman.o
FILETARGETS = files.o fileio.o filesearch.o filesort.o btree.o hash.o bitmap.o table.o
RELOPTARGETS = difference.o intersect.o nat_join.o projection.o rename.o selection.o union.o aggregation.o
OPTITARGETS = rel_eq_projection.o rel_eq_selection.o rel_eq_assoc.o rel_eq_comut.o

OUTDIR = ../bin

all: kalashnikov-db doc

kalashnikov-db: auxiliary.o $(OPTITARGETS) $(RELOPTARGETS) $(DISKTARGETS) $(MEMORYTARGETS) $(FILETARGETS) main.o
	$(CC) main.o $(OPTITARGETS) $(RELOPTARGETS) $(FILETARGETS) $(MEMORYTARGETS) $(DISKTARGETS) auxiliary.o -o $(OUTDIR)/kdb

main.o: main.c
	$(CC) $(CFLAGS) main.c

rel_eq_comut.o: rel_eq_comut.c
	$(CC) $(CFLAGS) rel_eq_comut.c

rel_eq_assoc.o: rel_eq_assoc.c
	$(CC) $(CFLAGS) rel_eq_assoc.c

rel_eq_selection.o: rel_eq_selection.c
	$(CC) $(CFLAGS) rel_eq_selection.c

rel_eq_projection.o: rel_eq_projection.c
	$(CC) $(CFLAGS) rel_eq_projection.c

aggregation.o: aggregation.c
	$(CC) $(CFLAGS) aggregation.c

union.o: union.c
	$(CC) $(CFLAGS) union.c

selection.o: selection.c
	$(CC) $(CFLAGS) selection.c

projection.o: projection.c
	$(CC) $(CFLAGS) projection.c

rename.o: rename.c
	$(CC) $(CFLAGS) rename.c

nat_join.o: nat_join.c
	$(CC) $(CFLAGS) nat_join.c

intersect.o: intersect.c
	$(CC) $(CFLAGS) intersect.c

difference.o: difference.c
	$(CC) $(CFLAGS) difference.c

bitmap.o: bitmap.c
	$(CC) $(CFLAGS) bitmap.c

btree.o: btree.c
	$(CC) $(CFLAGS) btree.c

hash.o: hash.c
	$(CC) $(CFLAGS) hash.c

files.o: files.c
	$(CC) $(CFLAGS) files.c

table.o: table.c
	$(CC) $(CFLAGS) table.c

filesort.o: filesort.c
	$(CC) $(CFLAGS) filesort.c
	
filesearch.o: filesearch.c
	$(CC) $(CFLAGS) filesearch.c

fileio.o: fileio.c
	$(CC) $(CFLAGS) fileio.c

memoman.o: memoman.c
	$(CC) $(CFLAGS) memoman.c

dbman.o: dbman.c
	$(CC) $(CFLAGS) dbman.c

auxiliary.o: auxiliary.c
	$(CC) $(CFLAGS) auxiliary.c

clean:
	rm -rf *~ *.o ../bin/kdb ../doc/* ../bin/kalashnikov.db

doc:
	doxygen doxygen.config
	cd ../doc/latex && make
