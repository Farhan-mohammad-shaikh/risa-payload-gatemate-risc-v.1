#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, Muhammad Bassam
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
    env.SetDefault(OS='rtems')

    env.SetDefault(COMPILERPREFIX='arm-rtems4.12-')

    # 'BSPPATH' shall be provided in the build environment settings
    env.SetDefault(BSPPATH='/opt/arm-rtems4.12/bsp/arm-rtems4.12/') 
    
    # When compiling the system for a ARMv6 or ARMv7-A/R processors,
    # the ARM Compiler will assume that it can use unaligned accesses.
    # for not using unaligned-access then '-mno_unaligned_access flag'
    # shall be set. (see http://infocenter.arm.com/help/topic/com.arm.doc.faqs/ka15414.html)
    
    env.SetDefault(CCFLAGS_target=['-march=armv7-a', '-mcpu=cortex-a9', 
                                   '-mthumb', '-mthumb-interwork', '-mfpu=neon', 
                                   '-mfloat-abi=hard', '-mtune=cortex-a9',
                                   '-qrtems', '--specs', 'bsp_specs',
                                   '-B$BSPPATH/lib', 
                                   '-B$BSPPATH/$BSP/lib'])
    env.SetDefault(CCFLAGS_optimize=['-O2', '-ffunction-sections', 
                                     '-fdata-sections'])

    env.SetDefault(CXXFLAGS_dialect=['-fno-rtti', '-fno-exceptions', ])

    env.Tool('settings_gcc_default_internal')


def exists(env):
    return env.Detect('gcc')
