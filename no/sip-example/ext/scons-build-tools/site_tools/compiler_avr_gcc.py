#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017-2018, Fabian Greif
# Copyright (c) 2023, Pascal Pieper
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

from SCons.Script import *


def generate(env, **kw):
    env['PROGSUFFIX'] = '.elf'
    env['ARCHITECTURE'] = 'avr'
    env.SetDefault(OS='none')

    env.SetDefault(COMPILERPREFIX='avr-')

    env.SetDefault(CCFLAGS_target=[])
    env.SetDefault(CCFLAGS_optimize=[
        '-Os',
        '-ffunction-sections',
        '-fdata-sections',
        '-finline-limit=10000',
        '-funsigned-char',
        '-funsigned-bitfields',
        '-fno-split-wide-types',
        '-fno-move-loop-invariants',
        '-fno-tree-loop-optimize',
        '-fno-unwind-tables',
        '-fshort-wchar',        # Required when using newlib.nano
        ])

    env.SetDefault(CXXFLAGS_optimize=[
        '-fno-threadsafe-statics',
        '-fuse-cxa-atexit', ])
    env.SetDefault(CXXFLAGS_language=[
        '-std=c++17',
        '-fno-exceptions',
        '-fno-rtti', ])

    env.SetDefault(LINKFLAGS_target=[])
    env.SetDefault(LINKFLAGS_optimize=[
        "-Wl,--relax",
        "-Wl,--gc-sections",
        ])

    env.SetDefault(ASFLAGS_other=[
        "-xassembler-with-cpp",
        ])

    env.Tool('settings_gcc_default_internal')


def exists(env):
    return env.Detect('gcc')
