###############################################################################
# Name		: BWToWord.py
# Author	: Stephen E. MacKenzie
# Copyright : Licensed under GPL version 3 (GPLv2)
###############################################################################

###############################################################################
# imports
import win32com
import win32clipboard
import sys
import re
import fileinput
import mymod
import bibleworks
import wordauto

###############################################################################
# namespace scoping
from sys import argv
from re import split
from win32com import client
from win32com.client import Dispatch
from win32clipboard import OpenClipboard
from win32clipboard import CloseClipboard
from win32clipboard import GetClipboardData
from mymod import strip_trailing_footnote
from mymod import split_keep_delims
from mymod import strip_nonalnum_re
from mymod import PrepreplaceFile
from bibleworks import BibleWorksAuto
from wordauto import WordAuto

###############################################################################
# Class for parsing the London Baptist Confession from raw text to Word
class LBCTextToWord:

	def __init__(self, refs = False, footnotesize = 16, bodysize = 18, 
								headingsize = 24, titlesize = 30):
		self.bw = None
		self.word = None
		self.just_refs = refs
		self.footnotesize = footnotesize
		self.bodysize = bodysize
		self.headingsize = headingsize
		self.titlesize = titlesize

	def IsJustRefs(self):
		return self.just_refs

	def ParseDashPart(self, bookchap, rest):
		verses = []
		dashtoks = rest.split('-')
	
		p = dashtoks.pop(0)
		if(p != None):
			low = int(p)
			p = dashtoks.pop(0)
			if(p != None):
				hi = int(p)
				for x in range(low, hi + 1):
					verses.append(bookchap + str(x))

		return verses					

	def PrintCommaVerses(self, bookchap, rest):
		savebookchap = bookchap
		toks = rest.split(',')
	
		for tok in toks:
			tok = strip_nonalnum_re(tok)
			bookchap += tok
			self.bw.GoToVerse(bookchap)
			s = self.bw.GetVerse()
			s = strip_trailing_footnote(s)
			self.PrintVerse(s)
			bookchap = savebookchap

	def PrintDashedVerses(self, verses, orig_dashed_ref):
		rest = ''
		for v in verses:
			self.bw.GoToVerse(v);
			s = self.bw.GetVerse()
			s = strip_trailing_footnote(s)
			#strip the 2 Timothy 3:, just get the verse number and verse
			toks = s.split(':')
			#just no need to save it
			toks.pop(0) 
			s = ''.join(toks)
			# now add verse number and verse text		
			#results.append(s)
			rest += ' ' + s + ' '
		
		self.word.PrintToWord(orig_dashed_ref + ': ', 1, self.footnotesize) 
		self.word.PrintToWord(rest, 0, self.footnotesize)

	def ParseVerse(self, verse):
		pattern = re.compile('^#[0-9]+')
		if(re.search(pattern, verse)):
			spaces = split_keep_delims(verse, ' ')
			footnum = spaces.pop(0)
			
			self.word.PrintToWord('\n', 1, self.footnotesize)
			self.word.PrintToWord(footnum, 1, self.footnotesize)
			verse = ''.join(spaces)

		stripped = strip_nonalnum_re(verse)

		#split by colon, first part is book and chapter, be sure to add the colon
		toks = stripped.split(':')
		bookchap = toks.pop(0)
		if(bookchap != None):
			bookchap += ':'

			rest = ''.join(toks)
			if(re.search('-', rest)):
				verses = self.ParseDashPart(bookchap, rest)
				if(verses != None):
					self.PrintDashedVerses(verses, stripped)
			elif(re.search(',', rest)):
				self.PrintCommaVerses(bookchap, rest)
			else:
				self.bw.GoToVerse(bookchap + rest)
				s = self.bw.GetVerse()
				s = strip_trailing_footnote(s)
				self.PrintVerse(s + ' ')

	def PrintVerse(self, verse):
		#break into two calls, set bold on first, add colon to first
		verse = verse[4:]
		toks = verse.split(':')
		bookchap = toks.pop(0)
		bookchap += ':'
		
		s = ''.join(toks)
		spaces = split_keep_delims(s, ' ')

		v = spaces.pop(0)
		v = strip_nonalnum_re(v)

		bookchap += v + ': '
		rest = ''.join(spaces)

		self.word.PrintToWord(bookchap, 1, self.footnotesize) 
		self.word.PrintToWord(rest, 0, self.footnotesize)
		

	def ParseVerseLine(self, verseline):
		if(self.just_refs == False):
			toks = verseline.split(';')
			for tok in toks:
				self.ParseVerse(tok)
		
			self.word.PrintToWord('\n')
		else:
			toks = verseline.split('#')
			for tok in toks:
				p = tok.replace(')', '')
				self.word.PrintToWord('#' + p + '\n')
	
	def ParseFile(self, file):
		PrepreplaceFile(file, 'Psalms', 'Psalm')

		if(self.IsJustRefs() == False):
			self.bw = BibleWorksAuto()
			self.bw.Initialize()

		self.word = WordAuto()
		self.word.Initialize()

		bv = re.compile('[0-9]+:[0-9]+')

		f = open(file)

		for line in f:	
			if('(' not in line):
				self.PrintNonVerseLine(line)
			else:
				toks = line.split('(')
				last = toks.pop()
				jline = '('.join(toks)
				self.PrintNonVerseLine(jline)
				self.ParseVerseLine(last)

		f.close()

	def PrintNonVerseLine(self, line):
		pattern = re.compile('^Chapter [0-9]+:')
		title = re.compile('^London Baptist Confession of Faith 1689')

		if(re.search(title, line)):
			self.word.PrintToWord(line, 1, self.titlesize)
		elif(re.search(pattern, line)):
			self.word.PrintToWord(line + '\n', 1, self.headingsize)
		else:
			self.word.PrintToWord(line, 0, self.bodysize)

	def RunWordMacro(self, name):
		self.word.RunMacro(name)

	def SaveWordDoc(self, name):
		self.word.SaveDoc(name)

	def SaveWordDocAsPDF(self, name):
		self.word.SaveDocAsPDF(name)

	def CloseWordDoc(self):
		self.word.CloseDoc()

###############################################################################
#  Global Driver functions		
def CreateOne(refs = False, footsize = 16, bodysize = 18, 
								headingsize = 24, titlesize = 30):
	lbc = LBCTextToWord(refs, footsize, bodysize, headingsize, titlesize) 
	lbc.ParseFile(argv[1])

	lbc.RunWordMacro('Normal.NewMacros.ReplaceHashes')
	lbc.RunWordMacro('Normal.NewMacros.ChapterHeadings1')
	lbc.RunWordMacro('Normal.NewMacros.DeleteStrayHashes')
	lbc.RunWordMacro('Normal.NewMacros.RestorePsalms')
	lbc.RunWordMacro('Normal.NewMacros.LBCTitle')
	lbc.RunWordMacro('Normal.NewMacros.RemoveExtraLines')
	lbc.RunWordMacro('Normal.NewMacros.ParagraphBoldItalic')

	if(lbc.IsJustRefs() == True):
		lbc.SaveWordDoc(argv[1] + '.logos' + '.docx')
	else:
		lbc.SaveWordDoc(argv[1] + '.print' + '.docx')
		lbc.SaveWordDocAsPDF(argv[1] + '.print' + '.pdf')

	lbc.CloseWordDoc()

###############################################################################
# main
def main():
	if(len(argv) > 2):
		if(argv[2] == "refs"):
			CreateOne(True)  #refs only for logos
		elif(argv[2] == "full"):
		#	CreateOne(False) #full verses for print book, default is booklet
			CreateOne(False, 11, 14, 18, 24)
	else:
		CreateOne(True)  #refs only for logos
		CreateOne(False) #full verses for print book

	print('BWToWord has finished executing\n')

###############################################################################
# call to main
main()

#for /R e:\PDF\ReformedBaptist\LBC\Templates %i in (*.txt) do type %i >> LBC.txt
#Word find and replace.  Record a macro.
#\#([0-9]{1,2})
#word replace
#\1
#be sure to set format to superscript, make sure to 
#highlight the replace field first