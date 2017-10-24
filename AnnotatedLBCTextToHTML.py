import re
import sys
import os
from sys import argv

def strip_nonalnum_re(word):
	return re.sub(r"^\W+|\W+$", "", word)

def main():
	assert(len(argv) == 2)

	f = open(argv[1])
	for line in f:
		#
		toks = line.split('.')
		line = re.sub(r'^(Paragraph [0-9]+)', r'<p><strong>\1.</strong>', toks.pop(0))
		print(line, end='')
		line = ''.join(toks)
		#
		k = line.rfind('(')
		s1 = line[:k] + '</br></br>'
		print(re.sub(r'#([0-9]+)', r'<sup>\1</sup>', s1), end='')
		s2 = '(' + line[k+1:]
		toks = s2.split('(')
		s2 = toks.pop(0)	
		s2 = toks.pop(0)
		s2 = strip_nonalnum_re(s2)
		s2 = re.sub(r'#([0-9]+)', r'</br><sup>\1</sup>', s2)
		s2 = s2 + ' </p>'
		print(s2)		
	
	f.close()
	
main()