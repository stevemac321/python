###############################################################################
# Name		: mymod.py
# Author	: Stephen E. MacKenzie
# Copyright : Licensed under GPL version 3 (GPLv2)
###############################################################################

###############################################################################

import sys
import re
import fileinput

###############################################################################
# namespace scoping
from sys import argv
from re import split

###############################################################################
# global utilities
def strip_trailing_footnote(verse):
	stripped = re.sub(r'\d+$', '', verse)
	return stripped

# For WTT, right align word.  Output WTT Gen 1:1:, Reverse, put to next line 
def reverse_words(line):
	s = ''
	toks = line.split(' ')
	while(toks):
		s += toks.pop()

	return s
		

def split_keep_delims(line, delim):
	return [e+delim for e in line.split(delim) if e]

def strip_nonalnum_re(word):
	return re.sub(r"^\W+|\W+$", "", word)

def PrepreplaceFile(file, find, repl):
	with fileinput.FileInput(file, inplace=True, backup='.sav') as file:
		for line in file:
			print(line.replace(find, repl), end='')