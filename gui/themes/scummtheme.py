#!/usr/bin/env python
# encoding: utf-8
import sys
import re
import os
import zipfile

def buildTheme(themeName):
	if not os.path.isdir(themeName):
		print "Invalid theme name: " + themeName
		return
	
	zf = zipfile.ZipFile(themeName + ".zip", 'w')
	
	print "Building '" + themeName + "' theme:"
	os.chdir(themeName)
	
	for filename in os.listdir('.'):
		if os.path.isfile(filename) and not filename[0] == '.':
			zf.write(filename, './' + filename)
			print "    Adding file: " + filename
			
	os.chdir('../')
	
	zf.close()

def buildAllThemes():
	for f in os.listdir('.'):
		if os.path.isdir(os.path.join('.', f)) and not f[0] == '.':
			buildTheme(f)
			
def parseSTX(theme_file, def_file):
	comm = re.compile("\/\*(.*?)\*\/", re.DOTALL)

	output = ""
	for line in theme_file:
		output +=  line.rstrip("\r\n\t ").lstrip() + " \n"
		
	output = re.sub(comm, "", output).replace("\t", " ").replace("  ", " ").replace("\"", "'").splitlines()
		
	for line in output:
		if line and not line.isspace():
			def_file.write("\"" + line + "\"\n")

def buildDefTheme(themeName):
	def_file = open("default.inc", "w")
	
	if not os.path.isdir(themeName):
		print "Cannot open default theme dir."
		
	for filename in os.listdir(themeName):
		filename = os.path.join(themeName, filename)
		if os.path.isfile(filename) and filename.endswith(".stx"):
			theme_file = open(filename, "r")
			parseSTX(theme_file, def_file)
			theme_file.close()
			
	def_file.close()
	
def printUsage():
	print "==============================="
	print "ScummVM Theme Generation Script"
	print "==============================="
	print "Usage:"
	print "scummtheme.py makeall"
	print "    Builds all the available themes.\n"
	print "scummtheme.py make [themename]"
	print "    Builds the theme called 'themename'.\n"
	print "scummtheme.py default [themename]"
	print "    Creates a 'default.inc' file to embed the given theme in the source code.\n"

def main():
		
	if len(sys.argv) == 2 and sys.argv[1] == "makeall":
		buildAllThemes()
		
	elif len(sys.argv) == 3 and sys.argv[1] == "make":
		buildTheme(sys.argv[2])
		
	elif len(sys.argv) == 3 and sys.argv[1] == "default":
		buildDefTheme(sys.argv[2])
		
	else:
		printUsage()

if __name__ == "__main__":
	sys.exit(main())
