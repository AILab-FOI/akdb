#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import division

import nltk
from collections import Counter
from nltk.tokenize import word_tokenize, sent_tokenize, RegexpTokenizer
from nltk.probability import FreqDist

# dat = raw_input("Enter .txt file name: ")
f = open("animalfarm.txt", 'r')
text = f.read()
f.close()

def countEllipsis():
	return text.count("... ") + text.count("...'") + text.count("...\n")

def statistics():
	wordCount = countWords()
	sentenceCount = countSentences()
	avgWordLength = avgLettersInWord()
	avgSentLength = avgWordsInSent()
	ellipsisCount = countEllipsis()

	print 
	print "Statistics:"
	print "Words in the document: ", wordCount
	print "Sentences in the document: ", sentenceCount
	print "Average word length: ", avgWordLength
	print "Average sentence length: ", avgSentLength
	print "Ellipsis occur ", ellipsisCount, " times."
	print

def countWords():
	tokenizer = RegexpTokenizer(r'\w+')
	t = tokenizer.tokenize(text)
	return len(t)

def countSentences():
	return len(sent_tokenize(text))

def avgLettersInWord():
	tokenizer = RegexpTokenizer(r'\w+')
	words = tokenizer.tokenize(text)
	avg = sum(len(word) for word in words)/len(words)

	return avg

def avgWordsInSent():
	t = sent_tokenize(text)
	# print t
	sentLengths = []

	for i in t:
		sentLengths.append(len(i.split(" ")))

	return sum(sentLengths)/len(sentLengths)

def bow():
	tokenizer = RegexpTokenizer(r'\w+')
	t = tokenizer.tokenize(text)
	counts = Counter(t)
	print counts

def bowPunctation():
	t = word_tokenize(text)
	print Counter(t)

def frequencyDistribution():
	t = word_tokenize(text)
	freq = FreqDist(t)
	print freq.most_common(50)

def tagging():
	test = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum."
	print nltk.pos_tag(word_tokenize(text))

ans=True
while ans:
		print ("""
		1. Bag of words without punctation
		2. Bag of words with punctation
		3. Frequency distribution (NLTK)
		4. Word tagging
		5. Statistics

		9. Tagset help
		0. Exit
		""")
		ans=raw_input("What would you like to do? ") 
		if ans=="1": 
			bow()
		elif ans=="2":
			bowPunctation()
		elif ans=="3":
			frequencyDistribution()
		elif ans=="4":
			tagging()
		elif ans=="5":
			statistics()
		elif ans=="9":
			nltk.help.upenn_tagset()
		elif ans=="0":
			print("\nBye bye")
			exit()
		elif ans !="":
			print("\n Not valid choice, try again.")