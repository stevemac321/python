import sys
import re
import mymod

from sys import argv
from mymod import strip_nonalnum_re

def sub_chapter(infile):

	f = open(infile)
	lines = 0
	
	for line in f:
		#
		toks = line.split('.')
		line = re.sub(r'^(Paragraph [0-9]+)', r'<p><strong>\1.</strong>', toks.pop(0))
		print(line, end='')
		line = ''.join(toks)
		#
		k = line.rfind('(')
		#s1 = line[:k]
		s1 = line[:k] + '</br>'
		print(re.sub(r'#([0-9]+)', r'<sup>\1</sup>', s1), end='')
		s2 = '(' + line[k+1:]
		toks = s2.split('(')
		s2 = toks.pop(0)	
		s2 = toks.pop(0)
		k = s2.rfind(')')
		s2 = s2[:k]
		s2 = re.sub(r'#([0-9]+)', r'</br><sup>\1</sup>', s2)
		s2 = s2 + ' </p>'
		print(s2)	
		lines += 1
	
	print('\n')
	print(r'<style>sup{line-height: 0}</style>')
	f.close()
	return lines

def usual():	
	assert(len(argv) == 3)
	chap = re.compile(r'^<h3>Chapter [0-9]+: ')

	f = open(argv[1])
	lines = 0
	
	for line in f:
		if(re.search(chap, line)):
			print(line, end='')
			lines = sub_chapter(argv[2])		
		else:
				if(lines > 0):
					lines -= 1;
				else:
					print(line, end='')
		
	f.close()

def main():
	usual()
	#sub_chapter(argv[1])	
	
main()