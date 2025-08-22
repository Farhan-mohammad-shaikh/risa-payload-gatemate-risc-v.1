#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, Fabian Greif
# Copyright (c) 2016, Jan-Gerd Mess
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

from SCons.Script import *
import sys

def generate(env, **kw):
    env['PROGSUFFIX'] = ''
    env['ARCHITECTURE'] = 'hosted'
    env['OS'] = 'posix'

    env.Tool('settings_gcc_default_internal')

    # Clang uses the same settings as GCC but requires a different naming
    # schema for the binutils
    prefix = env.get('COMPILERPREFIX', '')
    env['CC'] = prefix + 'clang'
    env['CXX'] = prefix + 'clang++'
    env['AS'] = prefix + 'llvm-as'
    env['OBJCOPY'] = prefix + 'objcopy'  # not available
    env['OBJDUMP'] = prefix + 'llvm-objdump'
    env['AR'] = prefix + 'llvm-ar'
    env['NM'] = prefix + 'nm'  # not available
    env['RANLIB'] = prefix + 'ranlib'  # not available
    env['SIZE'] = prefix + 'llvm-size'

    # On macOS LLVM is the default compiler, and does not have llvm- prefixes
    if sys.platform == "darwin":
        env['LINKFLAGS_other']=[]
        env['AS'] = prefix + 'as'
        env['OBJDUMP'] = prefix + 'objdump'
        env['AR'] = prefix + 'ar'
        env['SIZE'] = prefix + 'size'

    # No LLVM equivalent available, use the GCC version if requested.
    env['STRIP'] = 'strip'

    env['LINK'] = env['CXX']


def exists(env):
    return env.Detect('clang')
