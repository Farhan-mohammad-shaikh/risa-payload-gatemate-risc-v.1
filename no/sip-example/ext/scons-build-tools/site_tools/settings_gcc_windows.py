#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, Fabian Greif
# Copyright (c) 2018, Markus Schlotterer
# Copyright (c) 2018, Schlotterer, Markus
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
    env['PROGSUFFIX'] = '.exe'
    env['OS'] = 'none'

    # Try to find the boost library location
    boostdir = 'C:/boost/include'
    if os.path.exists(boostdir):
        env['BOOSTDIR'] = boostdir
    else:
        env['BOOSTDIR'] = 'D:/boost/include'

    env.Append(CPPPATH='$BOOSTDIR')
    env.Append(LIBPATH='$BOOSTDIR/../lib')


def exists(env):
    return True
