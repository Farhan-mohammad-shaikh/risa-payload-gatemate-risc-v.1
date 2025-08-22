#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016-2018, Fabian Greif
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

import os
import tempfile

from SCons.Script import *
import SCons.Subst

# We use an adapted Version of this class from 'SCons/Platform/__init__.py' for
# Windows because GCC requires all backslashes inside a parameter file to be escaped.
class TempFileMungeWindows(object):
    def __init__(self, cmd):
        self.cmd = cmd

    def __call__(self, target, source, env, for_signature):
        if for_signature:
            return self.cmd

        # do the expansion.
        cmd = env.subst_list(self.cmd, SCons.Subst.SUBST_CMD, target, source)[0]

        # create a file for the arguments
        fd, tmp = tempfile.mkstemp('.lnk', text=True)
        native_tmp = SCons.Util.get_native_path(os.path.normpath(tmp))

        args = list(map(SCons.Subst.quote_spaces, cmd[1:]))
        output = " ".join(args).replace("\\", "\\\\")
        os.write(fd, output + "\n")
        os.close(fd)

        if SCons.Action.print_actions and ARGUMENTS.get('verbose') == '1':
            print("TempFileMungeWindows: Using tempfile "+native_tmp+" for command line:\n"+
                  str(cmd[0]) + " " + " ".join(args))
        return [cmd[0], '@"' + native_tmp + '"\ndel', '"' + native_tmp + '"']


def generate(env, **kw):
    if str(Platform()) == "win32":
        # use a tempfile for the arguments, otherwise the command line string might be to long
        # for windows to handle (maximum length is 2048 characters)
        env['TEMPFILE'] = TempFileMungeWindows

    env['LINKCOM'] = "${TEMPFILE('%s')}" % env['LINKCOM']
    env['ARCOM'] = "${TEMPFILE('%s')}" % env['ARCOM']


def exists(env):
    return True
