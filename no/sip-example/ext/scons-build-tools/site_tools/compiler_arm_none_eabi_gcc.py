#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, Fabian Greif
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
    env['ARCHITECTURE'] = 'arm'
    env.SetDefault(OS='none')

    env.SetDefault(COMPILERPREFIX='arm-none-eabi-')
    env.SetDefault(CPU="cortex-m3")

    env.SetDefault(CCFLAGS_target=[
        '-mcpu=$CPU',
        '-mthumb',
        '-mthumb-interwork'])
    env.SetDefault(CCFLAGS_debug=['-gdwarf-2'])
    env.SetDefault(CCFLAGS_optimize=[
        '-Os',
        '-ffunction-sections',
        '-fdata-sections', ])
    env.SetDefault(CCFLAGS_other=[
        '-finline-limit=10000',
        '-funsigned-char',
        '-funsigned-bitfields',
        '-fno-split-wide-types',
        '-fno-move-loop-invariants',
        '-fno-tree-loop-optimize',
        '-fno-unwind-tables',
        '-fshort-wchar',        # Required when using newlib.nano
        ])

    env.SetDefault(CXXFLAGS_target=[
        "-mcpu=$CPU",
        "-mthumb",
        "-mthumb-interwork"])
    env.SetDefault(CXXFLAGS_other=[
        '-fno-threadsafe-statics',
        '-fuse-cxa-atexit',])
    env.SetDefault(CXXFLAGS_language=[
        '-std=c++17',
        '-fno-exceptions',
        '-fno-rtti',])

    env.SetDefault(ASFLAGS_target=[
        "-mcpu=$CPU",
        "-mthumb",
        ])

    env.SetDefault(LINKFLAGS_target=[
        "-mcpu=$CPU",
        "-mthumb",
        ])
    env.SetDefault(LINKFLAGS_optimize=['--gc-sections', ])
    env.SetDefault(LINKFLAGS_other=[
        "-Wl,--fatal-warnings",
        # "-Wl,-Map=project.map,--cref",
        ])

    env.Tool('settings_gcc_default_internal')


def exists(env):
    return env.Detect('gcc')
