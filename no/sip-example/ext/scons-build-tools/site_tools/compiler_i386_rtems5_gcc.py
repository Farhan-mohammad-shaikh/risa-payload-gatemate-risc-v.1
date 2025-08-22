#!/usr/bin/env python
#
# Copyright (c) 2020, Dennis Pfau
# Copyright (c) 2021, Jan Sommer
# Copyright (c) 2021, Sommer, Jan
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

import os

from SCons.Script import *


# -----------------------------------------------------------------------------
def generate(env, **kw):
    env.Append(ENV = {'PATH' : os.environ['PATH']})
    
    env.SetDefault(RTEMS_BSPS = os.getenv('RTEMS_BSPS', ''))
    
    if env['RTEMS_BSPS'] == '':
        print('\x1b[30;41m Error: \'RTEMS_BSPS\' not set. \x1b[0m')
        return
    
    env['PROGSUFFIX'] = ''
    env['ARCHITECTURE'] = 'x86'
    env['OS'] = 'rtems'
    env['COMPILERPREFIX'] =  'i386-rtems5-'
    env.Tool('settings_gcc_default_internal')
    
    env['CCFLAGS_target'] = [
       '-specs=bsp_specs',
       '-qrtems',
       '-march=pentium',
       '-mtune=pentiumpro',
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
    
    env['CXXFLAGS_language'] = [
        '-pedantic',
        '-fno-rtti',
        '-fno-exceptions',
    ]
    
    env.Append(LINKFLAGS_other=['-Wl,--gc-sections','-Wl,-Ttext,0x00100000'])

# ----------------------------------------------------------------------------- 
def exists(env):
    return env.Detect('gcc')
    
