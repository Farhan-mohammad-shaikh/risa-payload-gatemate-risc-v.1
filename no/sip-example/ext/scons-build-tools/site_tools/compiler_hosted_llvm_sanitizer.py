#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2017, Fabian Greif
# Copyright (c) 2022, Tobias Pfeffer
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

from SCons.Script import *


def generate(env, **kw):
    env.Tool('compiler_hosted_llvm')

    env['CCFLAGS_other'] = [
        '-fsanitize=undefined,address',
        '-fno-omit-frame-pointer',
        '-fno-optimize-sibling-calls'
    ]
    env['CCFLAGS_optimize'] = ['-O1']

    # Uses the clang static analyzer, see http://clang-analyzer.llvm.org/
    if ARGUMENTS.get('analyze') != None:
        env['CC'] = 'ccc-analyzer'
        env['CXX'] = 'c++-analyzer'
        env['CCFLAGS_optimize'] = ['-O0']

    # Enable initialization order checking
    # see https://clang.llvm.org/docs/AddressSanitizer.html#initialization-order-checking
    env['ENV']['ASAN_OPTIONS'] = 'check_initialization_order=1'

def exists(env):
    return env.Detect('clang')
