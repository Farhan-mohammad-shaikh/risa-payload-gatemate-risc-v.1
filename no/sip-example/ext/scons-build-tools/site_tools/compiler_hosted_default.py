#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, Markus Schlotterer
# Copyright (c) 2018-2019, Fabian Greif
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

from SCons.Script import *


def generate(env, **kw):
    if env.WhereIs('clang') and env.WhereIs('llvm-ar') is not None:
        # LLVM/Clang
        if env.get('USE_SANITIZER', False):
            env.Tool('compiler_hosted_llvm_sanitizer')
        else:
            env.Tool('compiler_hosted_llvm')
    else:
        # GCC
        env.Tool('compiler_hosted_gcc')
        env.Tool('settings_gcc_optionsfile')


def exists(env):
    return True
