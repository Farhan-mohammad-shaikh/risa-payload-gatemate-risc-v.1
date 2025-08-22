#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, Fabian Greif
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

from SCons.Script import *


def generate(env, **kw):
    env['PROGSUFFIX'] = ''
    env['ARCHITECTURE'] = 'or32'
    env['OS'] = 'none'

    env.SetDefault(COMPILERPREFIX='or32-aac-elf-')
    env.SetDefault(BOARD='urtu-312')

    env.SetDefault(CCFLAGS_optimize=['-O2', '-ffunction-sections', '-fdata-sections', ])
    env.SetDefault(CCFLAGS_target=['-mhard-mul', '-mhard-div', '-mhard-float', ])

    env.SetDefault(CXXFLAGS_dialect=['-fno-rtti', '-fno-exceptions', ])

    env.SetDefault(LINKFLAGS_optimize=['--gc-sections', ])
    env.SetDefault(LINKFLAGS_target=['-mboard=$BOARD', '-e256', ])

    env.Tool('settings_gcc_default_internal')


def exists(env):
    return env.Detect('gcc')

