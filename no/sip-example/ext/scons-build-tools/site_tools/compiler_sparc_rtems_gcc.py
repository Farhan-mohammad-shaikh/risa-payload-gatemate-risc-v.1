#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, Fabian Greif
# Copyright (c) 2016, Jan Sommer
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
    env['ARCHITECTURE'] = 'leon3'
    env['OS'] = 'rtems'
    
    env.SetDefault(COMPILERPREFIX='sparc-rtems-')

    # Without a '-q*' option, '-qleon3' is used (see Gaisler rcc-1.2.0
    # documentation) although it is not recognized when added as an explicit
    # parameter.
    env.SetDefault(CCFLAGS_target=['-mcpu=v8', '-msoft-float', ])
    env.SetDefault(CCFLAGS_optimize=['-O2', '-ffunction-sections', '-fdata-sections'])

    env.SetDefault(CXXFLAGS_dialect=['-fno-rtti', '-fno-exceptions', ])

    #env.SetDefault(LINKFLAGS_optimize=['-Wl,--gc-sections', ])

    env.Tool('settings_gcc_default_internal')


def exists(env):
    return env.Detect('gcc')
