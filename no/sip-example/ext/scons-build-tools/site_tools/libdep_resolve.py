#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2024, Pieper, Pascal
# Copyright (c) 2024, Felix Passenberg
# Copyright (c) 2024, Cold, Erin Cynthia
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

from typing import *
from SCons.Script import *
try:
    from libdep_resolve_internal import *
except ModuleNotFoundError:
    # FIXME: Fix import resolution. This is needed for unittests.
    # libdep resolve is not an actual module.
    from .libdep_resolve_internal import *


def insertModuleIntoEnv(env, module : str, *,
                        link_library_overrides : Mapping[str, str] = None,
                        print_level = 0,
                        explain = False):
    """
    Inserts header include path(s) and library into the environment
    so that targets are built against this module.
    It also parses "attributes", so tuples like ('pthread', 'external').

    \param link_library_overrides
        Map of library names that should be replaced by their respective value.
        Used at link-time for replacing implementations for testing.
    """
    current_indentation = print_level * 2 * '  '

    (modulename, module_attributes) = splitModuleNameAndAttributes(module)
    if module_attributes:
        if explain:
            print (current_indentation +
                   f" {modulename} has local attributes: '{module_attributes}'")

    if explain:
        print (current_indentation + f"Inserting module '{modulename}'")

    if 'attributes' in env and modulename in env['attributes']:
        global_attributes = makeAttributes(env['attributes'][modulename])
        module_attributes += global_attributes
        if len(global_attributes) > 0 and explain:
            print (f"{modulename} has global attributes: '{global_attributes}'")

    if len(getUnknownAttributes(module_attributes)) > 0:
        print (f"Libdep WARN: {modulename} has unknown attributes {getUnknownAttributes(module_attributes)}")

    # Insert library to link
    if ( not module_attributes or (
            not 'header_only' in module_attributes and
            not 'no_link' in module_attributes)
        ):
        module_to_link = modulename
        # check if the library should be manually replaced
        if (link_library_overrides and modulename in link_library_overrides):
            # this new module name might be 'None'
            module_to_link = link_library_overrides[modulename]
            if explain:
                print (current_indentation +
                        f"Replacing library '{modulename}' with '{module_to_link}'")

        env.Append(LIBS=[module_to_link])
    else:
        if explain:
            print (current_indentation + "Skipping append of library linking, " +
                   "because attributes 'header_only' or 'no_link' are set")

    if module_attributes is None:
        print (f"\nWarning: '{modulename}' was not registered using libdep, assuming external library!")
        print (f"\tTo suppress this warning, add dependency with attribute in a tuple like so: ('{modulename}', 'external')\n")

    # insert header / include paths
    if 'includepaths' in env and modulename in env['includepaths']:
            if explain: print (current_indentation +
                               "includepaths:")
            if env['includepaths'][modulename] is Ellipsis: # ellipsis is "irgendwie nichts oder so"
                if explain: print (current_indentation + '\t' +
                                   "No includepaths set")
            else:
                if explain:
                    for path in env['includepaths'][modulename]:
                        print (current_indentation + '\t' + str(path))
                if 'system' in module_attributes:
                    # Include the path with -isystem instead of -I
                    # TODO: Also the other flags possible:
                    # https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html
                    if explain:
                        print (f"{current_indentation}\t Module '{modulename}' has flag system set, so adding via -isystem")
                    for maybe_vardir_dir in env['includepaths'][modulename]:
                        path_to_include = str(env.Dir(maybe_vardir_dir).srcnode())
                        include_directive = f"-isystem {path_to_include}"
                        if explain:
                            print (f"{current_indentation}\t" + include_directive)

                        # AppendUnique does not work well with spaces in strings, so check manually
                        if not 'CXXFLAGS_other' in env or not path_to_include in env['CXXFLAGS_other']:
                            env.Append(CXXFLAGS_other=" " + include_directive)
                        if not 'CFLAGS_other' in env or not path_to_include in env['CFLAGS_other']:
                            env.Append(CFLAGS_other=" " + include_directive)
                else:
                    env.AppendUnique(CPPPATH=env['includepaths'][modulename])
                # TODO: possible inherited add / removal of defines through module_attributes here

# ----------- "public" functions -----------

def insertDependenciesIntoEnv(env,
                              modules : Union[MaybeAttributes, Iterable[MaybeAttributes]],
                              *,
                              root_module : MaybeAttributes = None,
                              link_library_overrides : Mapping[str, str] = None,
                              explain : bool = False):
    """
    `insertDependenciesIntoEnv()` looks into dependencies of \param modulename(s)
    and adds includepaths to CPPPATH of \param env and the name of the library
    to LIBS (if tuple `env['attributes'][modulename]'`
    does not contain 'header_only' or 'no_link').
    The dict of dependencies and the includepaths need to reside in the environment,
    to be added as it is done by `registerLibrary()`, and probably never done manually.

    \param root_module is the name of the module and just for cosmetics and debug printing.

    \param link_library_overrides is a map of library overrides, which will replace _all_
           (completely) matching library names with the replacement value (which may be None)
           during the environment dependency insertion. It will also try to resolve
           the dependencies of the overridden module.

    \param explain adds (extensive) debug print.
    """

    if not isinstance(modules, list):
        # Convenience function
        modules = [modules]

    if not link_library_overrides:
        link_library_overrides = {}

    identifiable_target_name = root_module
    if not identifiable_target_name:
        identifiable_target_name = f"Unknown target requiring {modules}"

    if explain: print (f'\nInserting {identifiable_target_name} and dependencies into environment')

    overridden_modules = applyLibraryOverrides(modules, link_library_overrides)

    if explain and overridden_modules != modules:
        print (f"There were overridden modules. Resulting list: {overridden_modules}")

    # generate transitive dependencies
    depmap = generateDependencyMap(env,
                                   modules= overridden_modules,
                                   link_library_overrides= link_library_overrides,
                                   explain= explain)

    if explain: print (f'\nGenerated dependency map: {depmap}\n')

    resolved_dependency_order = resolveDependencies(env,
                                                    dependency_map= depmap,
                                                    link_library_overrides= link_library_overrides,
                                                    root_modules=root_module,
                                                    explain= explain)

    if explain or ('verify' in env and env['verify']):
        if explain:
            print (f'Resulting (possibly) optimal order: {resolved_dependency_order}')
        list_to_check  = overridden_modules     # add modules to start to check for root dependencies
        list_to_check += resolved_dependency_order
        list_to_check += overridden_modules     # add modules again to supress errors on circular dependencies as this is only being compiled
        if not verifyDependencyOrder(env, modules + resolved_dependency_order + modules,
                                     library_overrides= link_library_overrides):
            raise Exception(f"Oopsie woopsie, the puter did a fuky wuki for '{identifiable_target_name}'!")

    for dependency in resolved_dependency_order:
        insertModuleIntoEnv(env, dependency, link_library_overrides=link_library_overrides, explain=explain)

    if explain:
        print (f'resulting CPPPATH:')
        for path in env['CPPPATH']:
            print (f'\t{path}')
        print (f'resulting LIBS:')
        for path in env['LIBS']:
            print (f'\t{path}')


def registerLibrary(env, modulename : str, files : list[str], headers = None,
                    *, # end of positional arguments
                    includepaths = None,
                    dependencies : DependencyList = None,
                    env_flag_overrides = None,
                    attributes : Attributes = None,
                    explain = False):
    """
    `registerLibrary` is the function called by module libraries.
    This creates a function that appends a function to
    `envGlobal['DELAYED_LIB_CREATION_FUNCTIONS']`.
    The main reason for this is to clone the individual environments only
    *after* all other modules have added their include headers and dependencies.
    This allows for a non-order-dependent inclusion of libraries,
    or even cyclic dependencies without littering the global environment.

    Modules / Libraries should call this in the form of:
    ```
    envGlobal.RegisterLibrary(modulename, files, headers, etc...)
    ```

    If building something more complicated, you can also append a different
    function that does something similar to `generateLibrary` with
    the single parameter `enviroment`, but you probably don't want that.

    \param files can be a list of everything that can be parsed by
           `objectifyMaybeWithFlags`, but most commonly only a list of filenames.

    \param includepaths is an optional list of paths to include-directories that
           might be used by dependent libraries.
           The path should be absolute.

    \param dependencies is a list of libraries that should be linked
           along with the key module or which provides needed include files.

    \param env_flag_overrides is a dict of environment changes to be performed
           only in this module's environment. For more detail on the syntax,
           please refer to `applyFlagOverrides(...)` documentation.

    \param attributes is a tuple of strings that affect modules that include this library.
           Valid options are a combination of:
           - 'system'
             Include paths are inserted using -isystem flag like
             https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html
           - 'external'
             Note that library that is not registered by Libdep, so suppress warning.
           - 'no_link', 'header_only'
             Library name is not an actual library but only a collection of include paths.
             Will suppress adding module name to `LIBS`.
    """

    # lol @python https://stackoverflow.com/questions/67480240/default-list-parameter-doesnt-reset
    if not headers:
        headers = []
    if not env_flag_overrides:
        env_flag_overrides = {}
    if not attributes:
        attributes = tuple()
    else:
        attributes = makeAttributes(attributes)
        if len(getUnknownAttributes(attributes)) > 0:
            print (f"Libdep WARN: {modulename} has unknown attributes {getUnknownAttributes(attributes)}")

    includepaths = flatten(includepaths)
    dependencies = flatten(dependencies)

    if 'external' in attributes:
        print (f"""registerLibrary for '{modulename}': Attribute 'external'
                makes no sense in registration!""")
        print ("attributes: " + str(attributes))


    if 'explain' in env and modulename in env['explain']:
        explain = env['explain'][modulename]

    if explain:
        attributes_str = ' ' + str(attributes) if len(attributes) > 0 else ''
        print (f"registerLibrary for '{modulename}'{attributes_str} with deps:")
        for lib in dependencies:
            (libname, attr) = splitModuleNameAndAttributes(lib)
            attributes_str = ' ' + str(attr) if len(attr) > 0 else ''
            print ("\t" + libname + attributes_str)

    if explain and len(env_flag_overrides) > 0:
        print (f'{modulename} has the following flag overrides:')
        for (key, value) in env_flag_overrides.items():
            print (f'\t{key}:{value}')

    env.Append(sources        = {modulename: files})
    env.Append(objects        = {modulename: []})
    env.Append(headers        = {modulename: headers})
    env.Append(dependencies   = {modulename: dependencies})
    env.Append(includepaths   = {modulename: includepaths})
    env.Append(attributes     = {modulename: attributes})
    env.Append(flag_overrides = {modulename: env_flag_overrides})
    env.Append(explain        = {modulename: explain})

    # create global registration list if not existing yet
    if not 'DELAYED_LIB_CREATION_FUNCTIONS' in env:
        env['DELAYED_LIB_CREATION_FUNCTIONS'] = {}

    if modulename in env['DELAYED_LIB_CREATION_FUNCTIONS']:
        raise Exception("Library {} was already registered".format(modulename))

    # Delay actual creation until all modules are registered
    env['DELAYED_LIB_CREATION_FUNCTIONS'][modulename] = generateLibrary


def registerStaticDependency(env, name : str,
                             link_libs : list[str] = None,
                             includepaths : list[str] = None, *,
                             attributes : tuple[str] = None):
    """
    Helper function to add a dependency to a target that may not be created using
    this libdep system, e.g. an external library that needs to be linked against
    another external library.
    If \param attributes contains 'header_only', it will not add \param name to libraries to link.
    """

    link_libs = flatten(link_libs)
    if not attributes:
        attributes = tuple()
    else:
        attributes = makeAttributes(attributes)

    if len(getUnknownAttributes(attributes)) > 0:
        print (f"Libdep WARN: {name} has unknown attributes {getUnknownAttributes(attributes)}")

    includepaths = flatten(includepaths)

    if 'external' in attributes:
        print (f"""Warning: registerLibrary for '{name}': Attribute 'external'
                makes no sense in registration!
                attributes: {str(attributes)}""")

    env.Append(sources        = {name: []})
    env.Append(headers        = {name: []})
    env.Append(dependencies   = {name: link_libs})
    env.Append(includepaths   = {name: includepaths})
    env.Append(attributes     = {name: attributes})
    env.Append(flag_overrides = {name: {}})
    env.Append(lib_overrides  = {name: {}})


def addTransitiveDependencyToModule(env, module : str,
                                    additional_dependencies : Iterable[MaybeAttributes]):
    """
    Used for adding dependencies to modules that already have been registered.
    This may be the case if a module declares a symbol, and one of many different
    implementations are chosen later.
    """

    if not 'dependencies' in env:
        raise Exception(f"Environment has no key 'dependencies'")

    if not module in env['dependencies']:
        raise Exception(f"Module '{module}' was not registered by libdep (yet?)")

    env['dependencies'][module] += flatten(additional_dependencies)


def generateLibrary(env, modulename, *, explain = False):
    """
    `generateLibrary` is (internally) used for delayed library creation,
    i.e. when all modules have registered their include paths and dependent libraries.
    Modules themselves will probably not need to call this, unless `registerLibrary`
    is not applicable for a special usecase, and a custom callback is created by the user.
    """

    if 'explain' in env and modulename in env['explain'] and env['explain'][modulename]:
        explain = True

    if explain:
        print ("\tGenerating library for " + modulename)

    # This is the delayed clone every comment was talking about.
    # All libraries should have been registered by now.
    libEnv = env.Clone()

    if 'flag_overrides' in libEnv and modulename in libEnv['flag_overrides']:
        flag_overrides = env['flag_overrides'][modulename]
        if len(flag_overrides) > 0:
            applyFlagOverrides(libEnv, flag_overrides, explain=explain)

    # Resolve all dependencies, but use this module's header only.
    # linking against ourselves does not make much sense.
    insertDependenciesIntoEnv(libEnv, (modulename, 'header_only'),
                              root_module=modulename, explain=explain)

    merged_objects = []
    for pre_obj in libEnv['objects'][modulename]:
        merged_objects.append(objectifyMaybeWithFlags(libEnv, pre_obj, explain=explain))
    for source in libEnv['sources'][modulename]:
        merged_objects.append(objectifyMaybeWithFlags(libEnv, source, explain=explain))

    env['objects'][modulename] = merged_objects    # override (maybe) changed objects

    # TODO: Add headers to static lib for external include? Is currently _not_ needed
    library = libEnv.StaticLibrary("$BUILDPATH/modules/" + modulename, merged_objects)
    libEnv.Install('$BUILDPATH/lib/', library)
    env.AppendUnique(LIBPATH=['$BUILDPATH/lib/']) # needs to be done only once, but globally


def generateAllRegisteredLibraries(env, *, explain = False):
    """
    Generates all registered libraries with the provided lambda.
    Removes the lambdas in the global variable 'DELAYED_LIB_CREATION_FUNCTIONS'
    to enable multiple "rounds" of independent library sets, e.g. FlightSW and Unittests.
    """

    if not 'DELAYED_LIB_CREATION_FUNCTIONS' in env:
        if explain:
            print ("no libraries were registered!")
        return

    if explain:
        print ("Generating {} registered libraries".format(len(env['DELAYED_LIB_CREATION_FUNCTIONS'])))

    for (modulename, libgen) in env['DELAYED_LIB_CREATION_FUNCTIONS'].items():
        libgen(env, modulename)

    # Delete generated libraries from dict
    env['DELAYED_LIB_CREATION_FUNCTIONS'] = {}


def printAllRegisteredDependencies(env, *,
                                   root_dependencies : list[str] = None,
                                   root_name : str = None):
    """
    Dependency debugging function.

    It takes one or many "root dependencies", which may be the
    dependencies of a library or a test.
    It will print out a deduced dependency tree, just as it would
    resolve the dependencies regularly, in mermaid syntax.
    You may render this using any mermaid renderer such as
    https://mermaid.live.

    TODO: Also print global attributes
    """

    if not 'dependencies' in env:
        print ("There are no registered dependencies.")
        return

    already_announced_modules = None
    print ("```mermaid")
    print ("flowchart\n")

    if root_dependencies:
        if isinstance(root_dependencies, str):
            root_dependencies = [root_dependencies]

        # announce the hierarchy first.
        # this tries to resemble a tree dependency graph

        # set out the layers so that we have a hierarchy
        depmap = generateDependencyMap(env, root_dependencies)
        layered_dependency_map = depmap.generateLayeredDependencyMap()

        transitive_dependencies = set()
        already_announced_modules = set()
        for depth, modules in layered_dependency_map.items():
            if root_name and depth == 0:
                print (f"subgraph {root_name}")
            else:
                print (f"subgraph layer{depth}")
            for module in modules.get():
                module_name = getModuleName(module)
                print (f"{module_name}")
                already_announced_modules.add(module_name)
                for dep in getDependenciesOfModule(env, module):
                    transitive_dependencies.add(getModuleName(dep))
            print ("end\n")

        # announce dependencies of dependencies. This should not happen in normal operation.
        remaining_transitive_dependencies = transitive_dependencies.difference(already_announced_modules)
        if len(remaining_transitive_dependencies) > 0:
            print (f"subgraph transitive")
            for tdep in remaining_transitive_dependencies:
                print (f"{tdep}")
                already_announced_modules.add(tdep)
            print ("end\n")

    # this just prints all (announced) dependencies, unordered
    for module, libs in env['dependencies'].items():
        module_name = getModuleName(module)

        if already_announced_modules:
            if not module_name in already_announced_modules:
                continue
        else:
            print (f"{module_name}")

        for dep in libs:
            dep_name, dep_attr = splitModuleNameAndAttributes(dep)
            attr_str = ""
            if len(dep_attr) > 0:
                attr_str = ", ".join(dep_attr)
            print (f"{module_name} --{attr_str}--> {dep_name}")

    print ("```")

# -----------------------------------------------------------------------------

def generate(env, **kw):
    env.AddMethod(insertDependenciesIntoEnv,
                 'InsertDependenciesIntoEnv')

    env.AddMethod(registerLibrary,
                  'RegisterLibrary')

    env.AddMethod(applyPreprocessor,
                  "ApplyPreprocessor")

    # Für die ganz speziellen Wünsche, wenn Sie verstehen
    env.AddMethod((lambda env, files: applyPreprocessor(env, files, 'LogObject')),
                  "ApplyLogPreprocessor")

    # probably not used by external users
    env.AddMethod(generateLibrary,
                  'GenerateLibraryCallback')

    env.AddMethod(generateAllRegisteredLibraries,
                'GenerateAllRegisteredLibraries')

    env.AddMethod(addTransitiveDependencyToModule,
                  'AddTransitiveDependencyToModule')

    env.AddMethod(registerStaticDependency,
                  'RegisterStaticDependency')

    env.AddMethod(printAllRegisteredDependencies,
                  'PrintAllRegisteredDependencies')

def exists(_):
    return True



