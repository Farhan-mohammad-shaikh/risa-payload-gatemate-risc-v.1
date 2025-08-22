#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, Fabian Greif
# Copyright (c) 2018, Markus Schlotterer
# Copyright (c) 2023, Toth, Norbert
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
    env['PROGSUFFIX'] = ''
    env['ARCHITECTURE'] = 'arm'
    env.SetDefault(OS='posix')

    env.SetDefault(COMPILERPREFIX='arm-linux-gnueabihf-')

    env.SetDefault(CCFLAGS_target=[
        '-march=armv6',
    ])

    env.SetDefault(CXXFLAGS_language=[
        '-std=c++17',
        '-fno-exceptions',
        '-fno-rtti',])

    env.Tool('settings_gcc_default_internal')


def exists(env):
    return env.Detect('gcc')
