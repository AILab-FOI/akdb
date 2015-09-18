#!/usr/bin/python
# -*- coding: utf-8 -*-

from __future__ import unicode_literals
from xgoogle.search import GoogleSearch, SearchError
import psycopg2
import sys

def export(results):
	if len(results) == 0:
		print "There is nothing to export"
		return
	else:
		dat = raw_input("Enter file name for exporting results: ")
		f = open(dat + ".txt", 'w')

		for res in results:
			f.write(res.title.encode("utf8"))
			f.write("\n")
			f.write(res.url.encode("utf8"))
			f.write("\n\n")
			
		f.close()
		print "Export: success."


def saveInDB(context):

	print "Connecting to db..."
	con = None

	try:
		con = psycopg2.connect("dbname='postgres' user='postgres' host='localhost' password='bla123'")
		con.autocommit = True
		cur = con.cursor()	

		print "Connected.\n"
		author = raw_input("Author: ")
		note = raw_input("Note: ")
		
		cur.execute("""insert into plagiarisms values(default, %s, %s, %s)""", (author, context, note))

	except psycopg2.DatabaseError, e:
		print 'Error %s' % e    
		sys.exit(1)
				
	finally:
		if con:
			con.close()


def dbSearch(context):
	print "Connecting to db..."
	con = None

	try:
		con = psycopg2.connect("dbname='postgres' user='postgres' host='localhost' password='bla123'")
		con.autocommit = True
		cur = con.cursor()	

		print "Connected. Searching DB...\n"

		cur.execute("select * from plagiarisms where context = %s", [context])
		# cur.execute("select * from plagiarisms")

		records = cur.fetchall()

		if len(records) == 0:
			print 'No results.'
			g = raw_input("Search on google? (y/n)")

			if g == 'y':
				googleSearch(context)
		else:
			print "Matching found: \n"
			print "Author: ", records[0][1]
			print "Context: ", records[0][2]	
			print "Note: ", records[0][3]
			print
			# print records



	except psycopg2.DatabaseError, e:
		print 'Error %s' % e    
		sys.exit(1)
				
	finally:
		if con:
			cur.close()
			con.close()

def googleSearch(context):

	try:
		gs = GoogleSearch(context)
		gs.results_per_page = 100
		results = gs.get_results()

		if len(results) == 0:
			print 'No results.'


		else:

			print 'Found ' + str(len(results)) + ' matches:'
			print results
			# print gs.num_results
			
			for res in results:
				print res.title.encode("utf8")
				# print res.desc.encode("utf8")
				print res.url.encode("utf8")
				print

			a = raw_input("Export search results in file? (y/n)")
			if a == 'y':
				export(results)
			else:
				print

			b = raw_input("Store in DB? (y/n)")
			if b == 'y':
				saveInDB(context)
				print
			else:
				print "Exiting."

	except SearchError, e:
		print "Error: %s" % e


def main():

	print
	# global context

	dat = raw_input("Enter .txt file name: ")
	f = open(dat + ".txt", 'r')
	context = f.read()
	f.close()

	dbSearch(context)


main()