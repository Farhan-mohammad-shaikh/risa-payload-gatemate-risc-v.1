#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2024, Pieper, Pascal
# Copyright (c) 2024, Pascal Pieper
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

"""
This utility tool adds easy unittest target and runner creation.
It also registers compilation databases.
It needs the `./libdep_resolve.py`.
"""

from typing import *
import os
import os.path
from libdep_resolve import insertDependenciesIntoEnv, objectifyMaybeWithFlags, applyFlagOverrides, flatten, printAllRegisteredDependencies

from SCons.Script import *

class TestDescription:
    def __init__(self,
                files : Iterable,
                dependencies : Iterable[str],
                path_to_module_src_root : str,
                env_flag_overrides : Mapping,
                link_library_overrides : Mapping,
                explain : bool = False
                ):
        self.files = files
        self.dependencies = dependencies
        self.path_to_module_src_root = path_to_module_src_root
        self.env_flag_overrides = env_flag_overrides
        self.link_library_overrides = link_library_overrides
        self.explain = explain

def generateTest(env, modulename, files, modulepath, dependencies = None, *,
                 env_flag_overrides = None, link_library_overrides = None,
                 explain = False, print_targets = False, **kwargs):
    """
    Unless more control is needed, this function will only be called internally.
    It generates a test executable and, dependent on some global variables,
    creates test runners and targets to run these for coverage
    and memcheck (llvm_asan, valgrind).
    This works only well if the files were included relative to the $BUILDPATH.
    The following extra parameters from \param kwargs usually come from
    `generateAllRegisteredTests(..., a = b)`
    """

    generateUnittest = kwargs.pop('generate_unittest_runtarget', True)
    unittestExtraFlags = flatten(kwargs.pop('unittest_extra_flags', []))
    unittestPrependString = kwargs.pop('unittest_prepend_string', '')
    unittestOutputXml = kwargs.pop('unittest_xml_outfile',
                            f"$BUILDPATH/modules/{modulename}/testresult.xml")
    generateCoverage = kwargs.pop('generate_coverage_runtarget', True)
    coverageExtraFlags = flatten(kwargs.pop('coverage_extra_flags', []))

    generateValgrind = kwargs.pop('generate_valgrind_runtarget', True)
    valgrindExtraFlags = flatten(kwargs.pop('valgrind_extra_flags', []))

    if len(kwargs) != 0:
        print (f"\tWarning utils_unittest: Unrecognized param passed: {str(kwargs)}")

    print_targets = print_targets or explain

    if dependencies is None:
        if explain: print (f'{modulename} does not have any dependencies set! defaulting to "self"')
        dependencies = [modulename] # this is a guess.

    # This is the delayed clone every comment was talking about.
    # All libraries should have been registered by now.
    runnerEnv = env.Clone()

    runner_name = modulename + '-test'

    if explain: print (f"Generating test for {runner_name}")

    insertDependenciesIntoEnv(runnerEnv, dependencies,
                                link_library_overrides= link_library_overrides,
                                root_module=runner_name,
                                explain = explain)

    if env_flag_overrides:
        applyFlagOverrides(runnerEnv, env_flag_overrides, explain=explain)

    if explain:
        print ("libraries to link:")
        for lib in runnerEnv['LIBS']:
            print ("\t" + lib)

    if print_targets:
        print ("\tTargets for module '" + modulename + "':")

    objects = [objectifyMaybeWithFlags(runnerEnv, file, explain=explain) for file in files]
    runner_with_path = os.path.join('$BUILDPATH', runner_name)
    runner = runnerEnv.Program(runner_with_path, objects)
    dir_of_runner = runner[0].get_dir().abspath
    path_to_runner = runner[0].abspath
    run_target_name = 'run-' + modulename

    runnerEnv.Alias(runner_name, runner)    # make unittest available by module name instead of path
    runnerEnv.Default(runner)               # build (only) unittests if no target is given
    if print_targets:
        print ("\t\t" + runner_name + "\t(build only)")

    # generate compilation database emission
    # this would be so much easier if the path filter would apply to the source dir files.
    # it builds up a prefix like 'modules/hal' from the *source* module path.
    relpath = os.path.relpath(modulepath, runnerEnv.Dir('#/..').abspath)
    runnerEnv['COMPILATIONDB_PATH_FILTER'] = os.path.join('*', relpath, '*')
    runner_database_name = runner_name + '_db'
    runnerEnv.Alias(runner_database_name, runnerEnv.CompilationDatabase(os.path.join('$BUILDPATH', modulename, 'compile_commands.json')))
    runnerEnv.Alias('unittests_db', runner_database_name)   # generic "all databases" alias
    if print_targets:
        print ("\t\t" + runner_database_name + "\t(build only)")

    # only the string, not yet a SCons target.
    runner_action = " ".join([path_to_runner] + unittestExtraFlags)

    delete_existing_coverage_data_action = ''
    if runnerEnv['coverage']:
        delete_existing_coverage_data_action = ' '.join([
            'find',
            dir_of_runner,
            # os.path.abspath(os.path.join(runnerEnv['CONTRIBPATH'], 'outpost-core')),
            '-name "*.gcda"',
            '-delete'
            ])

    if generateValgrind:
        if (not any('sanitizer' in tool for tool in runnerEnv['TOOLS'])):
            # valgrind unittest, only possible with non-shadow-memory binary (so not clang's ASAN)
            valgrind_output = os.path.join("$BUILDPATH", modulename, "valgrind.log")
            valgrind_action = " ".join([
                                        "valgrind",
                                        "--track-origins=yes",
                                        "--leak-check=full",
                                        "--fair-sched=yes",
                                        "--log-file=" + runnerEnv.File(valgrind_output).abspath,
                                        ] + valgrindExtraFlags + [  # valgrind exeucable name is positional (last)
                                        runner_action
                                    ])
            runner_valgrind = runnerEnv.Command(target = valgrind_output,
                                    source = path_to_runner,
                                    action = [delete_existing_coverage_data_action, valgrind_action]
                                    )
            valgrind_target_name = run_target_name + "-valgrind"
            if print_targets:
                print ("\t\t" + valgrind_target_name)
                if explain:
                    print ("\t\t\tvalgrind output expected at '" + runnerEnv.File(valgrind_output).abspath + "'")
            runnerEnv.Alias(valgrind_target_name, runner_valgrind)          # specific runner
            runnerEnv.Alias("run-unittests-valgrind", valgrind_target_name) # generic "run all runners" alias
        else:
            if explain:
                print ("Skipping valgrind target creation, because a different sanitizer-tool was detected:")
                for tool in runnerEnv['TOOLS']:
                    print ("{}{}".format(' -> ' if 'sanitizer' in tool else '    ', tool))
    else:
        if explain:
            print ("not generating valgrind runner because of argument:" + str(kwargs))

    if runnerEnv['coverage'] and generateCoverage:
        lcov_remove_pattern = ["/usr/*", "test/*", "default/*", "tools/*", "contrib/*"]
        src_path_absolute = runnerEnv.Dir("#").abspath      # '#' is SCon*struct* base path (/modules)
        modulepath_relative_to_src = os.path.relpath(modulepath, src_path_absolute)
        coverage_output_file = os.path.join("$BUILDPATH", modulename + ".xml")

        gcov_action = ' '.join([
                        'gcovr',
                        '--decisions',
                        '--root=' + src_path_absolute,
                        '--filter=' + modulepath_relative_to_src + '/src/', # filter is relative to root
                        '--object-directory=' + runnerEnv.Dir('$BUILDPATH').abspath,
                        ' '.join(['-e "' + pat + '"' for pat in lcov_remove_pattern]),
                        '--xml-pretty',     # Also emits xml
                        '-o ' + runnerEnv.File(coverage_output_file).abspath,
                        '--json ' + os.path.join(runnerEnv.Dir('$BUILDPATH').abspath, modulename + '.json')
                ] + coverageExtraFlags)
        gcov_runner = runnerEnv.Command(
            target = coverage_output_file,
            source = runner[0],
            action = [
                        delete_existing_coverage_data_action,
                        runner_action,
                        gcov_action,
                    ]
            )
        coverage_target_name = run_target_name + "-coverage"
        if print_targets:
            print ("\t\t" + coverage_target_name)
            if explain:
                print ("\t\t\tCoverage output expected at '" + runnerEnv.File(coverage_output_file).abspath + "'")
        runnerEnv.Alias(coverage_target_name, gcov_runner)              # specific runner
        runnerEnv.Alias("run-unittests-coverage", coverage_target_name) # generic "run all runners" alias
    else:
        if not generateUnittest:
            if explain:
                print ("not generating unittest runner because of argument:" + str(kwargs))
        else:
            if print_targets:
                print ("\t\t" + run_target_name)
            # default unittest
            gtest_output = runnerEnv.File(unittestOutputXml).abspath
            unittest_runner_action = ' '.join([
                        unittestPrependString,
                        runner_action,
                        ' --gtest_output=xml:' + gtest_output
            ])
            if explain:
                print ("\t\t\tUnittest result expected at '" + runnerEnv.File(gtest_output).abspath + "'")
            runner = runnerEnv.Command(
                    target = gtest_output,
                    source = runner[0],
                    action = unittest_runner_action
                )
            runnerEnv.Alias(run_target_name, runner)            # specific runner
            runnerEnv.Alias("run-unittests", run_target_name)   # generic "all runners" alias


def registerTest(env, testname, files, dependencies, *,
                 path_to_module_src_root = None, env_flag_overrides = None,
                 link_library_overrides = None,
                 explain = False):
    """
    This function registeres a test of a "module". A module is here only the folder where
    the tested source files reside.
    \param dependencies is a list of strings that key either to `libdep` modules
            or just plain system libraries to link.
    \param env_flag_overrides is a dict of environment overrides which is explained
            in detail in libdep_resolve's `applyFlagOverrides`.
    """
    if not 'REGISTERED_TESTS' in env:
        resetRegisteredTests(env)

    if not path_to_module_src_root:
        path_to_module_src_root = str(env.Dir("..").srcnode())

    env['REGISTERED_TESTS'][testname] = TestDescription(files, dependencies, path_to_module_src_root,
                                         env_flag_overrides, link_library_overrides, explain=explain)
    if explain:
        print ("Registered test '" + testname + "'")


def generateAllRegisteredTests(env, **kwargs):
    """
    This generates tests that have been registered previously.
    It expects the special global variable `env['REGISTERED_TESTS']` to be populated with
    names and their files, dependencies (strings), path to the module's sources, and the
    special environment override dict which is explained in detail in libdep_resolve's
    `applyFlagOverrides`.
    """

    explain = kwargs.pop('explain', False)

    if not 'REGISTERED_TESTS' in env:
        print ("Warning: No registered Tests found.")
        return
    if not 'GENERATED_TESTS' in env:
        env['GENERATED_TESTS'] = {}

    if explain:
        print ("Generating {} registered tests".format(len(env['REGISTERED_TESTS'])))

    for test, description in env['REGISTERED_TESTS'].items():
        if explain is not None:
            explain_module = test in explain or "all" in explain

        if not isinstance(description, TestDescription):
            raise RuntimeError("Libdep Unittest error: Registered description is not a valid TestDescription")

        if explain_module:
            print ("generateTest for '" + test + "' which src should be in " + description.path_to_module_src_root)
        generateTest(env, test, description.files, description.path_to_module_src_root, description.dependencies,
                     env_flag_overrides= description.env_flag_overrides,
                     link_library_overrides= description.link_library_overrides,
                     print_targets= "targets" in explain,
                     explain= explain_module or description.explain, **kwargs)
        # retain information about generated test for debugging
        env['GENERATED_TESTS'][test] = description

    # "consume" all tests
    resetRegisteredTests(env)

def resetRegisteredTests(env):
    """
    This resets registered tests, so that they won't be *generated* and thus built.
    """
    env['REGISTERED_TESTS'] = {}

def printDependenciesOfTest(env, testname : str):
    tests_to_search = {**env['REGISTERED_TESTS'], **env['GENERATED_TESTS']}
    if not testname in tests_to_search:
        print (f"Could not find given testname '{testname}' in registered or generated tests.")
        print ("Available tests:")
        for test in tests_to_search.keys():
            print (f"\t{test}")
        return

    description = tests_to_search[testname]

    # this should be the same as in the actual test generation.
    runner_name = testname + '-test'
    printAllRegisteredDependencies(env,
                                   root_dependencies = description.dependencies,
                                   root_name = runner_name)

# -----------------------------------------------------------------------------

def generate(env, **kw):

    # probably not used by the user, thus making him a un-user
    env.AddMethod(generateTest,
                 'GenerateTest')

    env.AddMethod(registerTest,
                 'RegisterTest')

    env.AddMethod(generateAllRegisteredTests,
                 'GenerateAllRegisteredTests')

    env.AddMethod(resetRegisteredTests,
                  'DiscardAllRegisteredTests')

    env.AddMethod(printDependenciesOfTest,
                  'PrintDependenciesOfTest')

def exists(_):
    return True



