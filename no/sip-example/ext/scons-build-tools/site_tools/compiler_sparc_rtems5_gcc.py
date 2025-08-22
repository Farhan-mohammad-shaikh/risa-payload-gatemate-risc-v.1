#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2017, Fabian Greif
# Copyright (c) 2016, Jan Sommer
# Copyright (c) 2020, Jan Malburg
# Copyright (c) 2023, Jan-Gerd Mess
# Copyright (c) 2023, Meß, Jan-Gerd
# Copyright (c) 2023-2024, Pascal Pieper
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

    env.SetDefault(COMPILERPREFIX='sparc-gaisler-rtems5-')

    env.SetDefault(CCFLAGS_target=['-qrtems','-mcpu=v8', '-mhard-float', '-mfix-gr712rc','-qbsp=gr712rc_smp'])
    env.SetDefault(CCFLAGS_optimize=['-O2', '-g', '-ffunction-sections', '-fdata-sections'])

    env.SetDefault(CXXFLAGS_dialect=['-fno-rtti', '-fno-exceptions', ])

    env.SetDefault(LINKFLAGS_optimize=['-Wl,--gc-sections'])

    env.Tool('settings_gcc_default_internal')

def exists(env):
    return env.Detect('gcc')
