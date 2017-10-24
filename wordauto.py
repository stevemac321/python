###############################################################################
# Name		: BWToWord.py
# Author	: Stephen E. MacKenzie
# Copyright : Licensed under GPL version 3 (GPLv2)
###############################################################################

###############################################################################
# imports
import win32com
import sys
import re
import fileinput

###############################################################################
# namespace scoping
from sys import argv
from re import split
from win32com import client
from win32com.client import Dispatch

###############################################################################
# Word automation class
class WordAuto:
	def __init__(self):
		self.word = None
		self.doc = None
		self.sel = None

	def Initialize(self):
		self.word = client.Dispatch('Word.Application')
		assert(self.word != None)	

		self.doc = self.word.Documents.Add()
		assert(self.doc != None)

		self.word.Visible = True
		self.sel = self.word.Selection

	def PrintToWord(self, line, bold = 1, fontsize=12, fontname = 'Palatino Linotype'):	
		self.sel.Font.Bold = bold
		self.sel.Font.Name = fontname
		self.sel.Font.Size = fontsize
		self.sel.TypeText(line)
	
	def SaveDoc(self, name):
		self.word.Application.ActiveDocument.SaveAs2(name)

	def SaveDocAsPDF(self, name):
		self.word.Application.ActiveDocument.SaveAs2(name, 17)

	def CloseDoc(self):
		self.word.Application.ActiveDocument.Close()

	def RunMacro(self, name):
		self.word.Application.Run(name)