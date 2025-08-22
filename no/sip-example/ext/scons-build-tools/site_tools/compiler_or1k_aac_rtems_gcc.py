#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, Fabian Greif
# Copyright (c) 2016, Jan-Gerd Mess
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

import os

from SCons.Script import *


def generate(env, **kw):
    env['PROGSUFFIX'] = ''
    env['ARCHITECTURE'] = 'or1k'
    env['OS'] = 'rtems'

    env.SetDefault(COMPILERPREFIX='or1k-aac-rtems4.11-')

    env.SetDefault(CCFLAGS_optimize=['-O2', '-ffunction-sections', '-fdata-sections', ])
    env.SetDefault(CCFLAGS_target=['-B${BSPPATH}', '-qrtems', '-specs', 'bsp_specs'])

    env.SetDefault(CXXFLAGS_dialect=['-fno-rtti', '-fno-exceptions', ])

    env.SetDefault(LINKFLAGS_optimize=['--gc-sections', ])

    builder_flash = Builder(
        action=Action("@make -C $NAND_PATH $TARGET PROGRAMMINGFILE=$SOURCE " \
                        "OUT=%s > /dev/null" % os.path.abspath(env["NAND_PATH"] + 'nandflash-program'),
                      cmdstr="$LSSCOMSTR"),
        suffix=".elf",
        src_suffix="")

    builder_copy = Builder(
        action=Action("cp $SOURCE $TARGET",
                      cmdstr="$INSTALLSTR"),
        suffix=".elf",
        src_suffix="")

    env.Append(BUILDERS = {
        'NandFlash' : builder_flash,
        'NandCopy' : builder_copy
    })

    env.Tool('settings_gcc_default_internal')


def exists(env):
    return env.Detect('gcc')
