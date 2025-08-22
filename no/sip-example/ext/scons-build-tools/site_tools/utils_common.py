#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, 2016-2018, Fabian Greif
# Copyright (c) 2018, Niklas Hauser
# Copyright (c) 2024, Pieper, Pascal
# Copyright (c) 2024, Janosch Reinking
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

import os
import os.path

from SCons.Script import *


def _listify(node):
    return [node, ] if (not isinstance(node, list) and
                        not isinstance(node, SCons.Node.NodeList)) else node


def remove_from_list(env, identifier, to_remove):
    """
    Remove strings from a list.

    E.g.
    env.RemoveFromList('CXXFLAGS_warning', ['-Wold-style-cast'])
    """
    if identifier.startswith('$'):
        raise Exception("Identifier '%s' must not start with '$'!" % identifier)

    l = env.subst('$' + identifier)
    if isinstance(l, str):
        l = l.split(' ')
    for r in _listify(to_remove):
        if r in l:
            l.remove(r)
    env[identifier] = l


def filtered_glob(env, pattern, omit=None, ondisk=True, source=False, strings=False):
    if omit is None:
        omit = []

    results = []
    for p in _listify(pattern):
        results.extend(filter(lambda f: os.path.basename(f.path) not in omit,
                              env.Glob(p)))
    return results


def findFilesThatAreInACertainFolderRecursively(env, foldernameToFind, filenameToFind, *,
                                                searchdir = None):
    """
    Helper script to recursively find files that reside in a certain folder.
    Used for finding library and test SConscripts.
    Returns *relative paths* so that basepath can be the source file's path
    and still include the VariantDir versions.
    """
    # search in source folder because python does not know about variant dir
    searchdir = '.' if searchdir is None else searchdir
    basepath = env.Dir(searchdir).srcnode().abspath
    relpaths = []
    for cwd, _, filenames in os.walk(basepath):
        if os.path.isdir(cwd) and os.path.basename(cwd) == foldernameToFind:
            for filename in filenames:
                if filename == filenameToFind:
                    fullpath = os.path.join(cwd, filename)
                    relpaths.append(
                        os.path.join(searchdir,
                                     os.path.relpath(fullpath, basepath))
                    )
    return relpaths


def findFilesWithExtensionRecursively(env, fileextensionToFind, *, searchdir = None):
    """
    Helper script to recursively find files with a certain file-ending.
    Used for finding source files.
    Returns *absolute paths in variant dir* so that searchdir can be
    the source file's path and still include the VariantDir versions.
    """
    # search in current script's source folder
    # but report relative to the script's invocation (probably in variant dir)
    # because python does not know about variant dir.
    searchdir = env.Dir('.') if searchdir is None else env.Dir(searchdir)
    if len(searchdir.variant_dirs) > 0:
        # we can access the "actual" variant dir
        basepath_vd = searchdir.variant_dirs[0].abspath
    else:
        # it's a guess, or searchdir is just not inside a variant dir.
        basepath_vd = searchdir.abspath
    basepath_src = searchdir.srcnode().abspath

    relpaths = []
    for cwd, _, filenames in os.walk(basepath_src):
        for filename in filenames:
            if filename.endswith(fileextensionToFind):
                fullpath = os.path.join(cwd, filename)
                # append (variant dir) basepath in case we did not start from "."
                relpaths.append(os.path.join(basepath_vd, os.path.relpath(fullpath, basepath_src)))

    if len(relpaths) == 0:
        print ("SCons: Warning: found no files ending with '{}' in folder '{}'!".
               format(fileextensionToFind, basepath_src))
    return relpaths


def list_symbols(env, source, alias='__symbols'):
    action = Action("$NM %s -S -C --size-sort -td" % source[0].path,
                    cmdstr="$SYMBOLSCOMSTR")
    return env.AlwaysBuild(env.Alias(alias, source, action))


def run_program(env, program):
    return env.Command('thisfileshouldnotexist',
                       program,
                       '@"%s"' % program[0].abspath)


def phony_target(env, **kw):
    for target, action in kw.items():
        env.AlwaysBuild(env.Alias(target, [], action))

# -----------------------------------------------------------------------------

def generate(env, **kw):
    env.Append(ENV={'PATH': os.environ['PATH']})

    env.AddMethod(remove_from_list, 'RemoveFromList')
    env.AddMethod(filtered_glob, 'FilteredGlob')

    env.AddMethod(findFilesThatAreInACertainFolderRecursively,
                  'FindFilesThatAreInACertainFolder')
    env.AddMethod(findFilesWithExtensionRecursively,
                  'RecursiveGlob')

    if ARGUMENTS.get('verbose') != '1':
        env['SYMBOLSCOMSTR'] = "Show symbols for '$SOURCE':"

    env.AddMethod(list_symbols, 'Symbols')

    env.AddMethod(run_program, 'Run')
    env.AddMethod(phony_target, 'Phony')

def exists(env):
    return True

