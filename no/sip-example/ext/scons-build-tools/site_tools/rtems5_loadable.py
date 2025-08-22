#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020, Jan Malburg
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

import sys
import os

from SCons.Script import *

def extractFilter(source , target, env, for_signature):
	if for_signature == True:
		return target
	writer = open(target[0].abspath, "w")
	reader = open(source[0].abspath, "r")
	i = 0
	for line in reader:
		i += 1
		if i>4 : # skip the first 4 lines they not interest us
			pos = line.find("0x")
			if pos > 0: # sanitiy check
				line = line[pos+19:]
				line = line[:line.find("(")]
				if line[0] != '_':
					writer.write("%s\n" % line)
	writer.close()
	return target

def generate_symbols(source, target, env, for_signature):
	if for_signature == True:
		return target
	flags = ""
	# traverse in the string   
	for ele in env['CXXFLAGS']:  
		flags += ele + " "
	for ele in env['CCFLAGS']:  
		flags += ele + " "
	return 'rtems-syms -e -f %s -c "%s " -o %s %s' % (source[1], flags, target[0], source[0])


# Creating ELF & binary
def createLoadableProgram(env, target, source):
	programPre = env.Program(target + '.pre', source)
	programMap = env.MapIntern(target + '.ms',programPre)
	programFilter = env.FilterIntern(target + '.filter' ,programMap)
	programSym = env.SymIntern(target + '_sym.o',[programPre,programFilter])
	result = env.Program(target, source + programSym)
	env.Clean(result, target + '.map')
	pos = target.rfind('.')
	if pos >= -1:
		tmp = target[:pos] + '.map'
		env.Clean(result, tmp)
	else:
		tmp = target + '.map'
		env.Clean(result, tmp)		
	return result

def link_loadable(source, target, env, for_signature):
    files = ""
    # traverse in the string   
    for ele in source:
    	files += ele.abspath + " "
    return 'sparc-gaisler-rtems5-ld -r %s -o %s' % (files, target[0])

def createLoadable(env, target, source):
    objects = env.Object(source)
    return env.LoadableInternal(target, objects)

# -----------------------------------------------------------------------------
def generate(env):
	# for objects that can be loaded
	loadableintern = Builder(generator = link_loadable,suffix = '.a')
	env.Append(BUILDERS = {'LoadableInternal' : loadableintern})
	env.AddMethod(createLoadable, "Loadable")
	# for programs that can load objects
	mapintern = Builder(action = 'rtems-syms -e -c "$CXXFLAGS $CCFLAGS" -m $TARGET -o /dev/null $SOURCE')
	env.Append(BUILDERS = {'MapIntern' : mapintern})
	filterintern = Builder(generator = extractFilter)
	env.Append(BUILDERS = {'FilterIntern' : filterintern})
	symintern = Builder(generator = generate_symbols)
	env.Append(BUILDERS = {'SymIntern' : symintern})
	env.AddMethod(createLoadableProgram, "ProgramLoadable")	

def exists(env):
    return True