#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2024, Pieper, Pascal
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

from typing import *
from SCons.Script import *

def flatten(maybe_nested_list_of_strings) -> list[str]:
    """
    Internal helper function to recursively flatten a (nested) list of strings.
    Similar to utils_common.py:_listify() but different in handling.
    """
    if maybe_nested_list_of_strings is None:
        return []

    if (isinstance(maybe_nested_list_of_strings, str) or
            isinstance(maybe_nested_list_of_strings, tuple)):
        return [maybe_nested_list_of_strings]

    if not hasattr(maybe_nested_list_of_strings, '__iter__'):
        # Not iterable, so maybe a scons object or similar
        print (type(maybe_nested_list_of_strings))
        return [maybe_nested_list_of_strings]

    flattened_list = []
    for sublist in maybe_nested_list_of_strings:
        flattened_list += flatten(sublist)

    return flattened_list

Attributes = tuple[str]
MaybeAttributes = Union[str, Attributes]
DependencyList = list[MaybeAttributes]
LibraryOverrides = Mapping[str, Optional[str]]

def splitModuleNameAndAttributes(maybeAttributeTuple : MaybeAttributes) -> tuple[str, Attributes]:
    if isinstance(maybeAttributeTuple, tuple):
        return (maybeAttributeTuple[0], tuple(maybeAttributeTuple[1:]))
    return (maybeAttributeTuple, tuple()) # name

def getModuleName(maybeAttributeTuple: MaybeAttributes) -> str:
    (name, _) = splitModuleNameAndAttributes(maybeAttributeTuple)
    return name

def makeAttributes(maybeAttributes : Attributes) -> Attributes:
    if isinstance(maybeAttributes, str):
        return (maybeAttributes, )
    return tuple(maybeAttributes)

def getAttributes(maybeAttributeTuple) -> Attributes:
    (_, attributes) = splitModuleNameAndAttributes(maybeAttributeTuple)
    return tuple(attributes)

def addAttribute(maybeAttributeTuple, attr_to_add : Union[str, Tuple[str]]) -> Attributes:
    name, attr = splitModuleNameAndAttributes(maybeAttributeTuple)
    return makeAttributes(name) + attr + makeAttributes(attr_to_add)

def getUnknownAttributes(attrs : Attributes) -> list[str]:
    known_attributes = {
        'system',       # add includes with -isystem
        'external',     # don't warn if it is not registered in libdep
        'header_only',  # Only include headers, don't add to linking list
        'no_link',      # same as 'header_only'
        'always_link',  # Force-insert library on dependency. Used with Complicated cyclic dependencies.
        'overridden'    # generated attribute if dependency was not resolved naturally but overridden.
    }
    unknown_attributes = []
    for attr in attrs:
        if not attr in known_attributes:
            unknown_attributes.append(attr)
    return unknown_attributes

def getDependenciesOfModule(env, module : MaybeAttributes) -> DependencyList:
    module_name = getModuleName(module)
    if module_name in env['dependencies']:
        return env['dependencies'][module_name]
    else:
        # empty
        return DependencyList()

def applyLibraryOverrides(modules : List[MaybeAttributes],
                         library_overrides : LibraryOverrides):
    overridden_modules = []
    for module in modules:
        if getModuleName(module) in library_overrides:
            overridden_module = library_overrides[getModuleName(module)]
            # it may be None!
            if overridden_module:
                name, attr = splitModuleNameAndAttributes(overridden_module)
                overridden_modules.append(makeAttributes(name) + attr + makeAttributes('overridden'))
        else:
            overridden_modules.append(module)
    return overridden_modules


def getOverriddenDependenciesOfModule(env,
                                      module : MaybeAttributes,
                                      library_overrides : LibraryOverrides) -> DependencyList:
    ordinary_dependecies = getDependenciesOfModule(env, module)
    if not library_overrides:
        return ordinary_dependecies

    return applyLibraryOverrides(ordinary_dependecies, library_overrides)

def getDependenciesOfModuleWithAttribute(env,
                                         module : MaybeAttributes,
                                         attribute_to_search : str) -> DependencyList:
    dependencies = getDependenciesOfModule(env, module)
    return [dep for dep in dependencies if attribute_to_search in getAttributes(dep)]

def objectifyMaybeWithFlags(env, fileObjectOrTupleWithFlags, *, explain = False):
    """
    This (internal) function makes a compileable SCons object out of a range
    of different element types.
    The default behavior is just to make an `SCons.Node` with `env.Object` out of it.
    But it also might be a tuple with flags (dict):
        1. If a key is 'PREPROCESSOR', it tries to apply the value of it as a function on `env`
           as an object generator. This works, e.g., with `LogObject` of outpost-satellite.
        2. All other keys are treated as **Keyword-Arguments** to the `Object`-generator.
           This is used for per-file environment changes, e.g.
           `("abc.cpp", {'CXXFLAGS_language': "-std=c++42"})`
    """
    if explain:
        print (str(fileObjectOrTupleWithFlags))

    # test whether unpack would be successful
    if hasattr(fileObjectOrTupleWithFlags, '__len__') and len(fileObjectOrTupleWithFlags) == 2:
        # technically, str of size 2 would also match. But what kind of filename would that be?
        (file, flags) = fileObjectOrTupleWithFlags  # Quak!
        if explain:
            print ("Is with Flags: " + str(file))
            for key, value in flags.items():
                print ("\t" + str(key) + " : " + str(value))

        # This is a magic key:
        if 'PREPROCESSOR' in flags.keys():
            preprocessor = getattr(env, flags['PREPROCESSOR'])
            if explain:
                print ("Found 'PREPROCESSOR' key: ")
                print ("      so applying it " + str(preprocessor))
            flags_without_pp = {k: v for k, v in flags.items() if 'PREPROCESSOR' not in k}
            return preprocessor(file, **flags_without_pp)
        else:
            # just has normal keys
            return env.Object(file, **flags)    # this applies the flags as key-values to object

    if type(fileObjectOrTupleWithFlags) is SCons.Node.NodeList: # or similar?
        # TODO. Somehow merge object's flags with env?
        if explain:
            print ("\tIs pre-set node list object")
        return fileObjectOrTupleWithFlags

    if explain:
        print (f'\t{type(fileObjectOrTupleWithFlags)}: Is simple or something that we can just shove into Object')
    return env.Object(fileObjectOrTupleWithFlags)

def applyPreprocessor(env, files, function):
    """
    Small helper function to add a function (name, as string) to every given file.
    Returns a list of files with \param function applied
    in the scheme that `objectifyMaybeWithFlags` expects.
    Throws an error if the requested function does not exist in `env`
    at the time of calling.
    """
    if not hasattr(env, function):
        raise Exception("'" + str(function) + "' is not defined in environment")
    return [(file, {'PREPROCESSOR' : function}) for file in flatten(files)]

def applyFlagOverrides(env, flags, *, explain = False):
    """
    Applies (possibly nested) dicts to the enviroment.
    Conceptually similar to objectifyMaybeWithFlags, but it applies flags into the Env
    instead of single Objects.

    Keys of the elements in \param[in] flags may also be functions on env, which will
    be applied with the value as parameter(s).

    Possible flags:
        1. If `key` is a defined function on env, like "RemoveFromList".
           E.g. `{'RemoveFromList' : ['CXXFLAGS_warning', ['-Wuseless-warning-1', '-Wuseless-warning-1']]}`
           will result in `env.RemoveFromList('CXXFLAGS_warning', ['-Wuseless-warning-1', '-Wuseless-warning-1'])`
           Notice the top-level '[]': These are used as individual arguments to the function.

        2. If the value of a key is a _string_, it is interpreted as a Scons key-value pair.
           E.g. `{'CXXFLAGS_language': '-std=c++14'}` will result in
           a **merge** of the values in env['CXXFLAGS_language'].
           Removal of an item in this value requires the function described above, because
           it has been decided to merge (i.e. append) instead of override / replace.

        3. If the value of a key is another _dict_, it is interpreted as a KWARGS parameter
           to the enviroment.
           E.g. `{'CPPDEFINES' : {'DOG_BARK': '"WOOF"'}}`
           will result in `env.AppendUnique(CPPDEFINES = {'DOG_BARK': '"WOOF"'})`;
           so in C-Sources it will be equivalent to `#define DOG_BARK "WOOF"`.


    This function might be merged with `objectifyMaybeWithFlags` if we find out
    how to deal with per-`SCons.Object` environments in the same way.
    """

    if explain:
        print ("Flags: " + str(flags))

    genericKeyFlags = {}
    genericParamFlags = {}
    for (key, value) in flags.items():
        if explain:
            print ("\t" + str(key) + ": " + str(value))
        if hasattr(env, key):
            if explain:
                print ("Is Method " + key + " with param " + str(value))
            getattr(env, key)(*value)
        else:
            if explain:
                print ("Is generic", end = " ")
                print (type(value),  end = " ")
            if isinstance(value, str):
                if explain:
                    print (" key")
                genericKeyFlags[key] = value
            else:
                if explain:
                    print (" param")
                genericParamFlags[key] = value

    if explain:
        print ("Before: ")
        for (k, _) in genericKeyFlags.items():
            print ("\t" + str(k) + ": " + (str(env[k]) if k in env else "--nonexistent--"))
        for (k, _) in genericParamFlags.items():
            print ("\t" + str(k) + ": " + (str(env[k]) if k in env else "--nonexistent--"))

    if len(genericKeyFlags) > 0:
        # pass-through dict to let it be applied as KWARGS in environment
        # This line decides to 'merge' instead of 'override'.
        env.MergeFlags(genericKeyFlags)
    if len(genericParamFlags) > 0:
        # apply flags as KWARGS directly
        env.AppendUnique(**genericParamFlags)

    if explain:
        print ("after: ")
        for (k, _) in genericKeyFlags.items():
            print ("\t" + str(k) + ": " + str(env[k]))
        for (k, _) in genericParamFlags.items():
            print ("\t" + str(k) + ": " + str(env[k]))

class UniqueDependencySet:
    def __init__(self, elements : MaybeAttributes = None) -> None:
        self.elements = []
        if elements:
            for elem in elements:
                self.add(elem)

    def get(self):
        return self.elements

    def add(self, elem : MaybeAttributes):
        """
        Unlike the standard function, this returns True if something was added.
        """
        if elem not in self.elements:
            self.elements.append(elem)
            return True
        return False

    def __str__(self) -> str:
        return str(self.get())

    __repr__ = __str__

ReverseDependencyMap = dict[str, UniqueDependencySet]
TaggedForwardDependencySet = dict[MaybeAttributes, int]
LayeredDependencyMap = dict[int, UniqueDependencySet]

def mergeTaggedSet(into : TaggedForwardDependencySet,
                   right : UniqueDependencySet,
                   depth : int):
    # tags the element with the lowest number, if available.
    for element in right.get():
        curr_depth = into[element] if element in into else depth
        into[element] = min(curr_depth, depth)

class BidirectionalDependencyMap:
    def __init__(self,
                forward : Union[TaggedForwardDependencySet, MaybeAttributes] = None,
                reverse : ReverseDependencyMap = None):
        if isinstance(forward, dict):
            self.forwardMap = forward
        else:
            self.forwardMap = dict()
            if forward:
                # is MaybeAttributes
                self.forwardMap[forward] = 0
        if not reverse:
            self.reverseMap = ReverseDependencyMap()
        else:
            self.reverseMap = reverse

    def generateLayeredDependencyMap(self) -> LayeredDependencyMap:
        ret = LayeredDependencyMap()
        for (module, layer) in self.forwardMap.items():
            if not layer in ret:
                ret[layer] = UniqueDependencySet()
            ret[layer].add(module)
        return ret

    def __str__(self) -> str:
        ret  = 'Forward: \n'
        for (dependency, depth) in self.forwardMap.items():
            ret += f'\n\t{dependency} ({depth}) '
        ret += f'\nReverse: '
        for target, wanted_by in self.reverseMap.items():
            ret += f'\n\t{target} <- {wanted_by}'
        return ret

def createOrAddElementToSetInMap(the_map : ReverseDependencyMap,
                                    key : str,
                                    value : str) -> bool:
    if not key in the_map:
        the_map[key] = UniqueDependencySet()
    # Returns true if we added a thing
    return the_map[key].add(value)

def getNextLevelOfDependencies(env,
                               modules : Iterable[str],
                               *,
                               link_library_overrides : Mapping[str, str] = None,
                               explain = False) -> tuple[UniqueDependencySet, ReverseDependencyMap]:
    """
    Determines all dependencies of the given modules (i.e. "next level").
    For efficiency, it returns only unique elements for the level,
    and prunes already resolved edges (i.e. dependencies).
    This is only correct if we assume that cyclic dependencies of modules
    are only one "connection" deep; meaning that the functions n of modules
    A and B are not like `a1 -> b1 -> a2 -> b2 -> ...`
    (but limited to the first three nodes in this example).
    If this is a problem that occurs, you should consider building a separate
    object-library called "AB" that contains all object files of these modules.

    Currently only used with `getUniqueDependencies`.
    """

    if not isinstance(modules, list):
        # if explain:
        #     print ("parameter `modules` is not a list, so trying to make one")
        if isinstance(modules, str):
            modules = [modules]

    if not link_library_overrides:
        link_library_overrides = {}

    if not 'dependencies' in env:
        raise ("Libdep: Error: There are no dependencies registered in the environment!")

    forward_dependencies = UniqueDependencySet()
    reverse_dependencies = ReverseDependencyMap()

    for module in modules:
        module_name = getModuleName(module)
        dependencies = getOverriddenDependenciesOfModule(env, module, link_library_overrides)
        if len(dependencies) == 0:
            if explain:
                print (f"\t'{module_name}' has no dependencies, so skipping" )
            continue

        if explain:
            print (f"\t'{module_name}' has {len(dependencies)} dependencies")

        for dependencyWithAttr in dependencies:
            (dependency_name, attributes) = splitModuleNameAndAttributes(dependencyWithAttr)
            if explain:
                attrstring = f": {attributes}" if len(attributes) > 0 else ""
                print (f'\t\t{dependency_name} {attrstring}')
            forward_dependencies.add(dependencyWithAttr)
            createOrAddElementToSetInMap(reverse_dependencies, dependency_name, module_name)

    return (forward_dependencies, reverse_dependencies)

def generateDependencyMap(env,
                          modules : Union[MaybeAttributes, Iterable[MaybeAttributes]], *,
                          link_library_overrides : LibraryOverrides = None,
                          explain = False):

    if not isinstance(modules, list):
        modules = [modules]

    dependency_map = BidirectionalDependencyMap()
    current_layer = UniqueDependencySet()
    for module in modules:
        current_layer.add(module)
        dependency_map.forwardMap[module] = 0

    level = 0
    while True:
        level += 1
        if explain:
            print (f'Level {level}')

        (curr_dependencies, reverse) = getNextLevelOfDependencies(env,
                                                                  modules= current_layer.get(),
                                                                  link_library_overrides=link_library_overrides,
                                                                  explain= explain)

        if explain:
            print (f'  returned dependencies: {curr_dependencies}')
            print (f'                reverse: {reverse}')

        prev_num_dependencies = len(dependency_map.forwardMap)
        added_a_reverse_dependency = False
        #
        mergeTaggedSet(dependency_map.forwardMap, curr_dependencies, level)
        for (module, needed_by) in reverse.items():
            for dep in needed_by.get():
                if createOrAddElementToSetInMap(dependency_map.reverseMap,
                                            module, dep):
                    added_a_reverse_dependency = True

        aftr_num_dependencies = len(dependency_map.forwardMap)
        if aftr_num_dependencies == prev_num_dependencies and not added_a_reverse_dependency:
            if explain: print ('We converged, Feierabend!')
            break

        # advance "current" dependency layer
        current_layer = curr_dependencies

    return dependency_map

def findFirstOccurrenceOfElement(haystack : Iterable, needle) -> Union[int, None]:
    for i, hay in enumerate(haystack):
        if needle == hay:
            return i
    return None

def findLastOccurrenceOfElement(haystack : Iterable, needle) -> Union[int, None]:
    maybe_reversed_idx = findFirstOccurrenceOfElement(reversed(haystack), needle)
    if maybe_reversed_idx is None:
        return None
    return (len(haystack) - 1) - maybe_reversed_idx

def findFirstOccurrenceOfElements(haystack : Iterable, elementsToFind : Iterable) -> Iterable[Union[int, None]]:
    indexes = []
    for needle in elementsToFind:
        indexes.append(findFirstOccurrenceOfElement(haystack, needle))
    return indexes

def getNonexistentElements(elements : Iterable, indexes : Iterable[int]):
    if len(elements) != len(indexes):
        print (f"elements: {elements}, indexes: {indexes}")
        raise Exception("Invalid parameter")
    ret = []
    for i, occurence_index in enumerate(indexes):
        if occurence_index is None:
            ret.append(elements[i])
    return ret

def resolveDependencies(env,
                        dependency_map : BidirectionalDependencyMap,
                        link_library_overrides : Mapping[str, str] = None,
                        root_modules : Union[str, List[str]] = None,
                        explain : bool = False) -> DependencyList:
    # TODO: the link_list could be initialized with env['LIBS'] to have global knowledge
    link_list = []
    layered_dependency_map = dependency_map.generateLayeredDependencyMap()
    if explain:
        print (f"Given dependency map has levels {layered_dependency_map.keys()}")

    starting_depth = max(layered_dependency_map.keys())

    # "root-modules" are somewhat special.
    # Sadly, in a world with cyclic dependencies,
    # sometimes the currently-being-built module
    # is required by some other module that is being
    # inserted later.
    # announcing a module as "root" will result in
    # it not being emitted but still its dependencies resolved.
    # Also, layer 0 modules may either be dependencies of something different
    # (called "root_requirement" here) or the actual root element that is
    # being built. Meh.
    if not root_modules:
        root_modules = []
    if isinstance(root_modules, str):
        root_modules = [root_modules]

    for depth in range(starting_depth, -1, -1): # reverse enumerating
        if explain:
            print (f'Depth {depth}')
        if not depth in layered_dependency_map:
            continue

        modules = layered_dependency_map[depth].get()
        for module in modules:
            module_name = getModuleName(module)

            wanted_by = []

            if depth == 0 and module_name not in root_modules:
                # depth == 0 is a root requirement and should always de added
                wanted_by.append("root_requirement")
            if module_name in dependency_map.reverseMap:
                wanted_by.extend(dependency_map.reverseMap[module_name].get())

            if len(wanted_by) == 0 and module_name not in root_modules:
                print (f'Libdep [Bug]: {module_name} is in dependency list, but is wanted by no other module?')

            wants = getOverriddenDependenciesOfModule(env, module, link_library_overrides)

            if explain:
                print (f'\t{module}')
                print ('\t' + (' ' * len(str(module))) + f'wants -> {wants}')
                print ('\t' + (' ' * len(str(module))) + f'wanted by <- {wanted_by}')
                print (f'Current (reverse!) list: {link_list}')

            always_wants = getDependenciesOfModuleWithAttribute(env, module, "always_link")
            if len(always_wants) > 0:
                if explain:
                    print (f"\tAdding dependencies with 'always_link': {always_wants}")
                link_list.extend(always_wants)

            this_modules_effective_link_list = link_list
            newest_emit_position = findFirstOccurrenceOfElement(link_list, module)
            if newest_emit_position is not None:
                if explain:
                    print (f"\tThis module was last emitted at idx {newest_emit_position}")
                this_modules_effective_link_list = link_list[newest_emit_position:]

            if explain:
                print (f'\tThis module\'s link_list:\n\t    {this_modules_effective_link_list}')

            # -- find unsatisfied Dependencies
            oldest_occurrences_of_deps = findFirstOccurrenceOfElements(this_modules_effective_link_list, wants)
            # # oldest_occurrences_of_rev_deps = findFirstOccurenceOfElements(link_list, wanted_by)
            unsatisfied_deps = getNonexistentElements(wants, oldest_occurrences_of_deps)
            if len(unsatisfied_deps) > 0:
                if explain: print (f'\tThere are yet unsatisfied dependencies: {unsatisfied_deps}')
                unsat_deps_that_are_root_modules = findFirstOccurrenceOfElements(
                                                        haystack=root_modules,
                                                        elementsToFind=unsatisfied_deps)
                if explain:
                    print (f"{unsat_deps_that_are_root_modules} found for {unsatisfied_deps} in {root_modules}")
                unsat_deps_to_add = getNonexistentElements(elements=unsatisfied_deps,
                                                            indexes=unsat_deps_that_are_root_modules)
                unsat_deps_to_add = UniqueDependencySet(unsat_deps_to_add)

                if explain: print (f'\tadding the following non-root modules: {unsat_deps_to_add}')

                # Very ugly: Vor every unsat dep, check whether they have a requried link dependency
                for unsat_dep in unsat_deps_to_add.get():
                    nested_forced_deps = getDependenciesOfModuleWithAttribute(env, unsat_dep, 'always_link')
                    for nested_forced_dep in nested_forced_deps:
                        unsat_deps_to_add.add(nested_forced_dep)
                    # This could be configured to be N levels deep. But please don't.

                link_list.extend(unsat_deps_to_add.get())
            # ---

            oldest_occurrences_of_rev_deps = findFirstOccurrenceOfElements(link_list, wanted_by)

            later_modules = getNonexistentElements(wanted_by, oldest_occurrences_of_rev_deps)
            if explain: print (f'\tNot yet emitted modules that depend on this one: {later_modules}')
            if len(later_modules) == 0:
                # so, big reveal! If all modules that need this module were already emitted,
                # then inserting this module here might be pointless!
                if explain:
                    print (f"\tAll modules that need '{module_name}' were already emitted. Skipping.")
            else:
                # at least one modules that need this one will come.
                # so give them what they need, later.
                if explain:
                    print (f"\tat least one modules that need this one will come. Adding '{module_name}'.")
                link_list.append(module)
            if explain:
                print (f'After (reverse!) list: {link_list}')
                print ()

    # Add headers, which should be available.
    # this is done afterwards to not confuse the reverse dependency supression.
    # sorry, this is seriously complicated
    for root_module in root_modules:
        header_only_module = addAttribute(root_module, "header_only")
        if explain:
            print (f"Adding root_module as header_only: {header_only_module}")
        link_list.append(header_only_module)

    forward_link_list = list(reversed(link_list))

    return forward_link_list

def verifyDependencyOrder(env,
                          dependency_list : DependencyList,
                          library_overrides : LibraryOverrides = None,
                          explain : bool = False):
    alreadyCheckedModules = set()
    ret = True
    for i, module in enumerate(dependency_list):
        processedElements = dependency_list[:i]
        remainingElements = dependency_list[i:]
        remainingElementsNames = [getModuleName(elem) for elem in remainingElements]

        if explain:
            print (f"Processed Modules: {processedElements}")
            print (f"Remaining Modules: {remainingElements}")

        module_name = getModuleName(module)

        if module_name in alreadyCheckedModules:
            if explain:
                print (f'Already checked module "{module_name}", so skipping')
            continue

        this_modules_dependencies = getOverriddenDependenciesOfModule(env, module_name, library_overrides)

        if explain:
            print (f'Checking {module_name} <- {this_modules_dependencies}')

        # make sure that all dependencies exist at least once in the remaining elements
        for dependency in this_modules_dependencies:
            dependency_name = getModuleName(dependency)
            if not dependency_name in remainingElementsNames:
                print (f"\n[Libdep] Error:")
                print (f"{module_name} needs {this_modules_dependencies}, but {dependency_name} is missing!" )
                print (f"Processed Modules: {processedElements}")
                print (f"Remaining Modules: {remainingElementsNames}")
                print (f'(needs to appear in remaining modules)')
                ret = False

        alreadyCheckedModules.add(module_name)

    return ret
