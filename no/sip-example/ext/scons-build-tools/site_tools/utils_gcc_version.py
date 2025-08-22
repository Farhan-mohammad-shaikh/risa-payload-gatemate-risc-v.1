#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, Fabian Greif
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

import re
import subprocess


def detect_gcc_version(env, gcc=None):
    """"Detect the version of the used GCC.

    Used env['CXX'] as reference. A version string such as 4.4.3 is
    transformed into an integer with two characters per level, here: 40403.

    Examples:
      4.9.2         -> 40902
      4.7           -> 40700
      4.6.5         -> 40605
      4.5.3-or32-1  -> 40503
      4.3.10        -> 40310
    """""
    if gcc is None:
        gcc = env['CXX']

    v = subprocess.Popen([gcc, '-dumpversion'], stdout=subprocess.PIPE).stdout.read().decode("utf-8")
    # v = 4.5.3-or32-1
    version = re.match("^(\d)\.(\d)\.(\d)(-(.*))$", v)
    if version:
        compiler_version = int(version.group(1)) * 10000 + \
                           int(version.group(2)) * 100 + \
                           int(version.group(3))
    else:
        # Compiler version could not be detected
        compiler_version = 0

    return compiler_version


def generate(env, **kw):
    env.AddMethod(detect_gcc_version, 'DetectGccVersion')


def exists(env):
    return True
