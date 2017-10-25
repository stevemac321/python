import re
import sys
from sys import argv

assert(len(argv) > 1)
file = open(argv[1])

comma = re.compile(',')
colon = re.compile(':')

misformed = re.compile(r'[0-9]+:[0-9]+-[0-9]+,')
double_colon = re.compile(r'[0-9]+:[0-9]+:[0-9]+')

for line in file:
	toks = line.split('#')
	for tok in toks:
		if(re.search(comma, tok) and re.search(colon, tok)):
			print(argv[1] + ': ' + tok)
			


for line in file:
	toks = line.split('#')
	for tok in toks:			
		if(re.search(misformed, tok)):
			print('Misformed: ' + argv[1] + ': ' + tok)


for line in file:
	toks = line.split('#')
	for tok in toks:			
		if(re.search(double_colon, tok)):
			print('double_colon: ' + argv[1] + ': ' + tok)

			
file.close()