## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

kalashnikov_dbdocdir = ${prefix}/doc/kalashnikov-db
kalashnikov_dbdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS \
	op_preimenovanje.c

EXTRA_DIST = $(kalashnikov_dbdoc_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

