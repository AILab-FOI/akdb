# encoding=utf8
"""@package docstring
Documentation comments.py module.

Basically this module is detecting a language of a file and making comments for the same. 
"""
import re, sys, time, codecs
from langdetect import detect
import subprocess
from subprocess import Popen, call

commentsFile = "all_comments.tmp"
cFiles = []
pyFiles= []

"""
Documentation for getcommentsFiles() function

This function is searching for file that ends with either .py extension or .c extension and 
 appending the same in constant cFiles/pyFiles.
"""
def getcommentsFiles():
	with open("files.txt", "r") as fi:
		for f in fi:
			if f.rstrip().endswith(tuple("py")):
				pyFiles.append(f.rstrip()[2:])
			else:
				cFiles.append(f.rstrip()[2:])
				
"""
Documentation for a function detectLanguage().
 
 Function is detecting language (is ti croatian od sl) of a newly created commentsFile.
"""
def detectLanguage():
	print "Analyzing " + str( len(cFiles) + len(pyFiles) ) + " files. This may take a while."
	
	failed = []

	with codecs.open(commentsFile, "r", encoding='utf8') as f:
	    lineNum = 1
	    for line in f:
	        lineNum += 1
	    	if len(line) < 10 or "author" in line:
	    		pass
	    	else:
		        try:
	        		if detect(line.encode('utf-8')) == 'hr' or detect(line.encode('utf-8')) == 'sl':
	        			failed.append(line)
	        	except:
	        		pass

	print "Found " + str(len(failed)) + " suspicious comments: "
	for fail in failed:
		print fail

	print "Finding suspicious comments...done."


	# erase contents of the comments file?
	# maybe delete the file?
	open(commentsFile, 'w').close()

	# return len(failed)

"""
Documentation for makeComentsFile() function.

Function is parsing comments from file with .c extension and .py extension
"""
def makeCommentsFile():
	print "Making comments file..."

	# parse comments from all .c files
	command = ['sh', './parseC.sh']
	command.extend(cFiles)
	subprocess.call(command)

	time.sleep(1) # because of reasons

	# get comments from all .py files
	command = ['sh', './parsePy.sh']
	command.extend(pyFiles)
	subprocess.call(command)

	print "Done."

# main
getcommentsFiles()
makeCommentsFile()
detectLanguage()