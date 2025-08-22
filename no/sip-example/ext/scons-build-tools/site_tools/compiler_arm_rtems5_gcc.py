#!/usr/bin/env python
#
# Copyright (c) 2020, Dennis Pfau
# Copyright (c) 2020, Sommer, Jan
# Copyright (c) 2021, Jan Sommer
# Copyright (c) 2023, Pascal Pieper
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

import os

from SCons.Script import *

def strip_binary(env, target, source, options="--strip-unneeded"):
    return env.Command(target,
                       source,
                       Action("$STRIP %s -o %s %s" % (options, target, source[0]),
                              cmdstr="$STRIPCOMSTR"))
# -----------------------------------------------------------------------------
def generate(env, **kw):
    env.Append(ENV = {'PATH' : os.environ['PATH']})

    env.SetDefault(RTEMS_BSPS = os.getenv('RTEMS_BSPS', ''))

    if env['RTEMS_BSPS'] == '':
        print('\x1b[30;41m Error: \'RTEMS_BSPS\' not set. \x1b[0m')
        return

    env['PROGSUFFIX'] = ''
    env['ARCHITECTURE'] = 'arm'
    env['OS'] = 'rtems'
    env['COMPILERPREFIX'] =  'arm-rtems5-'
    env.Tool('settings_gcc_default_internal')

    env['CCFLAGS_target'] = [
       '-specs=bsp_specs',
       '-qrtems',
       '-march=armv7-a',
       '-mthumb',
       '-mfpu=neon',
       '-mfloat-abi=hard',
       '-mtune=cortex-a9',
    ]

    # Add the BSP specific include and library files
    for path in filter(None, env['RTEMS_BSPS'].split(':')):
        path = os.path.join(path, env['BOARD'])
        if not os.path.isdir(path):
            print('\x1b[30;41m No BSP found for ' + env['BOARD'] +
              ' in ' + path + '. Is \'RTEMS_BSPS\' set?\x1b[0m')
            return
        env.Append(CPPPATH=[os.path.join(path, 'lib/include')])
        env.Append(LIBPATH=[os.path.join(path, 'lib')])
        env.Prepend(CCFLAGS_target=['-B' + os.path.join(path, 'lib')])

    env.Append(LIBS=['rtemsbsp', 'rtemscpu', 'z', 'jffs2'])


    env['CCFLAGS_optimize'] = ['-O2', '-g','-ffunction-sections', '-fdata-sections', '-Wall']
    env.RemoveFromList('CFLAGS_language', '-std=c99')

    env.Append(LINKFLAGS_other=[
        '-Wl,-Map,${TARGET.base}.map',
        '-Wl,--gc-sections',
    ])

    env.AddMethod(strip_binary, 'Strip')

# -----------------------------------------------------------------------------
def exists(env):
    return env.Detect('gcc')


