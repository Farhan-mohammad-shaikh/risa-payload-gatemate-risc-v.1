
SCons Tool Collection
=====================

The SCons tools are grouped by prefixing the tool name.

Note:
The group prefixes should ideally be folders. Unfortunately this
is not supported by SCons for all target platforms.


Group 'compiler'
----------------

Compiler named `compiler_hosted_*` compile for the host operating
system, all others are cross-compiler. The cross-compilers follow the
GCC naming schema:

   arch_(vendor_)(os_)(abi_)gcc

Available tools:

- compiler_hosted_gcc
- compiler_hosted_gcc_coverage
- compiler_hosted_llvm
- compiler_hosted_llvm_sanitizer
- compiler_arm_none_eabi_gcc
- compiler_sparc_rtems_gcc
- compiler_or1k_aac_rtems_gcc
- compiler_arm_rtems5_gcc
- compiler_i386_rtems5_gcc
- compiler_or32_aac_elf_gcc


### Build flags

The standard Unix build flags like `CCFLAGS` are split into several
environment variables. With this the cross compiler can set several
options while allowing the user to change other parts.

The following environment variables are available:

Options for C and C++ (`CCFLAGS`):

- CCFLAGS_target
- CCFLAGS_optimize
- CCFLAGS_debug
- CCFLAGS_warning
- CCFLAGS_other

Options for C++ (`CXXFLAGS`)

- CXXFLAGS_language
- CXXFLAGS_dialect
- CXXFLAGS_warning
- CXXFLAGS_other

Options for C (`CFLAGS`)

- CFLAGS_language
- CFLAGS_dialect
- CFLAGS_warning
- CFLAGS_other

Options for the linker (`LINKFLAGS`)

- LINKFLAGS_target
- LINKFLAGS_other

Typically `*_target`, `*_dialect` and `*_warning` are set by the
cross compilers, while the others variables can be freely changed by
the user. It is also possible to overwrite the definitions from the
cross-compiler by explicitly specifying the value.

Example for overriding a flag:

    env = Environment(toolpath=[...],
                      tools=['compiler_hosted_gcc'],
                      CXXFLAGS_language=['-std=c++20', '-pedantic'])
    ...
    or
    ...
    env['CXXFLAGS_language'] = ['-std=c++20', '-pedantic']  # this will override the list

### Group 'settings'

The setting tools configure the environment.

Available tools:

- setttings_buildpath
- setttings_gcc_default_internal
- setttings_gcc_optionsfile

The `setttings_gcc_default_internal` tool is not intended to be used
by the user but is loaded by the GCC based compilers to define a common
set of options.

If the `setttings_gcc_optionsfile` tool is loaded the GCC command line
options are passed in a temporary file to avoid problems with over-long
command line arguments (especially under Windows).

With `setttings_buildpath` it is possible to perform out-of-source
builds. The tool must be loaded **after** the compiler because it alters
the emitters for object files and libraries. The build folder can be
specified by setting the `BUILDPATH` and `BASEPATH` environment
variables.

### Group 'utils'

Tools in this folder add helper functions for writing SConstruct files.

Available tools:

- utils_common
- utils_buildformat
- utils_buildsize
- utils_gcc_version
- utils_unittest


The `utils_buildformat` tool defines `COMSTR*` variables for the
standard tools generating a cleaner command line output. The actual
command line options can be show by using `scons verbose=1`.

`utils_buildsize` is useful for embedded devices. It shows an overview
of the used Flash (ROM) and RAM areas. The user need to specify sizes
for Flash and RAM.

Example:

    env = Environment(toolpath=[...],
                      tools=['compiler_sparc_rtems_gcc', 'utils_buildsize'],
                      DEVICE_SIZE={
                          'name' : 'Nexys 3',
                          'flash': 16777216,
                          'ram'  : 16777216
                      },)
    ...

With the `utils_gcc_version` tool it is possible to detect the used
GCC version. Has to be loaded after the compiler. It is loaded by
default from all GCC based compilers.

#### `utils_unittest`

The `utils_unittest` adds a generic way of building and running unittests.
It needs `libdep` to work, but at the end collects and runs unittests
plain, with valgrind, and coverage (and compilation databases).
It detects what is possible to construct and (optionally) prints the available
targets for each mode.
The target `run-unittests` ( plus `-coverage` and `-valgrind`) are collections
and run all registered tests.
Like `libdep`, it consists of one or more test registration and generation phases.
If a project uses submodules that register tests, a `DiscardAllRegisteredTests()`
is needed before registration of project-local tests, if it is not desired to run
all of the submodule's tests as well.
Usually, because tests may come with stubs that are useful to the base project,
the submodule's `SConscript.test` are called, then discarded, and then the own
`SConscript.test` is called.

Examples:

1.  In unittest `SConscript`s, you would use `RegisterTest`:

    ```python
    unittest_deps = [
        module,
        module + '_stubs',
        'outpost_base_stubs',
        'googletest-local',
    ]
    files = envGlobal.RecursiveGlob('.cpp', searchdir = 'outpost')
    files += [envGlobal.File('main.cpp')]
    envGlobal.RegisterTest(module, files, unittest_deps)
    ```

2.  In your main `SConstruct`, use `GenerateAllRegisteredTests`.
    It has the parameter `explain` (which is default set to silent).
    If `explain` is set to a prefix or complete name of a test / module,
    it will explain in more detail where to expect files and how the decisions
    were based.
    It may also be set to `'targets_only'`, where it will print all available
    targets to run.

    ```python
    # libs are build in the same path for all modules
    envGlobal.SConscript('$BUILDPATH/SConscript.library', exports='envGlobal')
    envGlobal.SConscript('$BUILDPATH/SConscript.test', exports='envGlobal')

    envGlobal.GenerateAllRegisteredTests(explain = "targets_only") # can also be a module name
    ```

    Example output:
    ```
    Generating 7 registered tests
        Targets for module 'outpost_log-decode':
            outpost_log-decode-test     (build only)
            outpost_log-decode-test_db  (build only)
            run-outpost_log-decode
        ...
    ```

    The function `GenerateAllRegisteredTests` also has keywords to prepend / append to
    unittest, coverage, and valgrind parameters.
    They can be used, e.g., if a gcov or Clang sanitizer suppression is needed:

    ```python
    envGlobal.GenerateAllRegisteredTests(
            coverage_extra_flags = [
            "--exclude-unreachable-branches",
            "--gcov-ignore-parse-errors=negative_hits.warn"
            ],
            unittest_prepend_string =
            'env UBSAN_OPTIONS=suppressions=' + envGlobal.File('$TASKINGPATH/tasking.suppress').abspath
        )
    ```

### Libdep

The Libdep module abstracts some of the SCons intricates with delayed environment clones away.
It adds the concept of libraries and their dependencies, and enables more complicated dependency schemes [^1].
It adds of one or more "registration" and "generation" phases, in where libraries first state
their dependencies and a recipie how to build the actual library (just like modern CMake).
The actual collection of all environment parameters is generated in the generation phase,
when all dependencies have also registered their parameters.


Examples:

1. Build an _executable_ with already registered libraries:
    ```python
    files = envGlobal.RecursiveGlob('.cpp')
    deps = [
        'outpost_base'             # see point 2.
        ('weird_lib', 'external'), # see point 6.
    ]
    env.InsertDependenciesIntoEnv(deps)
    env.Program('mycoolfun', files)
    ```

2. Register a library:

    ```python
    modulename = "outpost_" + os.path.basename(os.path.abspath('../'))

    includepaths = os.path.abspath('.')

    files = envGlobal.RecursiveGlob('.cpp')
    impl_files = envGlobal.RecursiveGlob('_impl.h')
    deps = [
        'outpost_utils',
    ]
    envGlobal.RegisterLibrary(modulename, files, impl_files,
                            includepaths = includepaths, dependencies = deps)
    ```

    And later, usually in the `SConscript.library`
    ```python
    envGlobal.GenerateAllRegisteredLibraries() # optionally with explain = 'my_lib', or True
    ```

3. If you have a file-specific compiler setting, you may add the file in a tuple:

    ```python
    files = envGlobal.RecursiveGlob('.cpp', searchdir = 'outpost')
    files += [('special.cpp', {'CXXFLAGS' : '-DMACROVAL=1337'})]
    ```

    The special key `'PREPROCESSOR'` will try to call the given value as a function
    to generate the resulting Object:

    ```python
    files = envGlobal.RecursiveGlob('.cpp', searchdir = 'outpost')
    # files = [(file, {'PREPROCESSOR' : 'logObject'}) for file in files]
    # or better, use the convenience function:
    files = env.applyPreprocessor(files, 'logObject')
    ```

4. If you have a per-library / test specific setting, you can use
   `libdep`s `env_flags_override` (see also `libdep_resolve.py:applyFlagOverrides()`)

    ```python
    extra_flags = {}    # don't do anything on default
    if '-std=c++17' in envGlobal['CXXFLAGS_language']:      # if this string is set
        extra_flags['CXXFLAGS_language'] = '-std=c++20'     # add a new item to list
        extra_flags['RemoveFromList'] = ['CXXFLAGS_language', '-std=c++17'] # ... and remove c++17

    files = envGlobal.RecursiveGlob('.cpp', searchdir = 'outpost')
    files += [envGlobal.File('main.cpp')]

    envGlobal.RegisterTest(module, files, unittest_deps, env_flag_overrides = extra_flags)
    ```

    Possible flags:
    1. If `key` is a defined function on env, like "RemoveFromList":
        E.g. `{'RemoveFromList' : ['CXXFLAGS_warning', ['-Wuseless-warning-1', '-Wuseless-warning-1']]}`
        will result in `env.RemoveFromList('CXXFLAGS_warning', ['-Wuseless-warning-1', '-Wuseless-warning-1'])`
        Notice the top-level '[]': These are used as individual arguments to the function.

    2. If the value of a key is a _string_, it is interpreted as a Scons key-value pair.
        E.g. `{'CXXFLAGS_language': '-std=c++14'}` will result in
        a **merge** of the values in to `env['CXXFLAGS_language']`.
        Removal of an item in this value requires the function described above, because
        it has been decided to merge (i.e. append) instead of override / replace.

    3. If the value of a key is another _dict_, it is interpreted as a KWARGS parameter
        to the enviroment.
        E.g. `{'CPPDEFINES' : {'DOG_BARK': '"WOOF"'}}`
        will result in `env.AppendUnique(CPPDEFINES = {'DOG_BARK': '"WOOF"'})`;
        so in C-Sources it will be equivalent to `#define DOG_BARK "WOOF"`.

5.  If you have external libraries that you don't want to create using `libdep`,
    you can register them using `registerStaticDependency()`.
    Just like `RegisterTest()`, it allows the addition of `attributes`.
    In this case, `'header_only'` and `'system'` are useful.
    For more information, see next point 5.


    ```python
    envGlobal.RegisterStaticDependency('icecream-local',
                                        includepaths = os.path.abspath('icecream-0.3.1'),
                                        attributes = ('header_only', 'system'))

    libs_to_link += ['icecream-local']

    envGlobal.RegisterStaticDependency('googletest-local',
                                       link_libs = libs_to_link,
                                       includepaths = googletest_path,
                                       attributes = 'system')
    ```

6.  It is possible to add attributes to libraries and dependencies.
    `'header_only'` (and `'no_link'`) suppresses the inclusion of the given name
    into the `LIBS` structure of Scons, thus not trying to link but only include
    the given `includepaths`.
    `'system'` will include the paths with `-isystem <path>` instead of the default
    `-I <path>` during compliation, which suppresses warnings found in these files.
    Note, that this is compiler specific and should not be done for code you have
    under control.

    ```python
    deps = [
        ('some_extlib', 'system'),
        ('atomic', 'external'),
        ('its_complicated', 'always_link'),
    ]
    envGlobal.RegisterLibrary("my_module", files, impl_files,
                              includepaths = includepaths, dependencies = deps,
                              attributes = ('header_only'))
    ```

    This will 1. include `some_extlib` paths here and in every Env that depends
    on `my_module` via `-isystem` (and link it).
    It will 2. link a possibly unknown `atomic` library without failing / warning.
    If some Env _depends_ on `my_module`, it will not attempt to link a library
    called `my_module`, but still link the dependencies `some_extlib` and `atomic`.
    (and include possibly set includepaths).
    Lastly, it will 3. always add the library `its_complicated` to the link list
    when the `my_module` is linked. This can be used for closely coupled libraries
    in parallel dependency situations.

    In special occasions, e.g. when a library is an interface which is implemented by
    a different library, it may be necessary to alter the dependency of a library after
    registration.
    This can be done with `AddTransitiveDependencyToModule`:

    ```python
    envGlobal.AddTransitiveDependencyToModule(module= 'outpost_core',
                                              additional_dependencies= "outpost_core_stubs")
    ```

7.  If a library that was registered as a dependency, and the developer (for testing)
    needs to replace said library link target, `link_library_overrides` can be used.
    It is a map of library overrides, which will replace _all_ (completely) matching
    library names with the replacement value (which may be `None`) during the environment dependency insertion.

    ```python
    deps = [
        'my_big_lib',   # has dependencies where we want to replace some
    ]
    library_overrides = {
        'some_lib': 'my_testing_lib',     # will be replaced
        'some_other_lib': None,           # will not be linked, e.g., because
                                          # 'testing_lib' also provides some of these lib's symbols
    }
    env.InsertDependenciesIntoEnv(deps, link_library_overrides=library_overrides)
    env.Program('mycoolfun', files)
    ```

8.  Dependency printing: As Libdep has to track dependencies anyways,
    it also offers a visualization for debugging or project manangement.
    It prints a [`mermaid`](mermaid.live) flavored markdown source
    that can be visualized externally.
    You can either print all known modules and their dependencies
    or specifically layered tree starting from a set of your program's
    dependencies.

    ```python
    # prints all known modules
    envGlobal.PrintAllRegisteredDependencies()

    # prints hierarchical dependency tree, starting from `deps`
    deps = [
        'my_big_lib',
    ]
    envGlobal.PrintAllRegisteredDependencies(root_dependencies = deps, root_name = "mycoolprog")
    ```

[^1]: As dependent libraries might be shared between dependencies,
      it will probably be included multiple times, but guaranteed to be bounded.
      The reason for this seemingly redundant multiple inclusion is that libraries
      might have circular dependencies or other funky stuff. Additionally, the Linker
      will throw away all symbols that have not been requested at the time of the
      inclusion of the currently looked-at library.
      E.g. Module A has symbol a_1 and needs Module B's b_1, and module B needs a_1
      but provides b_1, the (sad) optimal linking procedure is A,B,A (or B,A,B).
      This is considered and leads to non-pruned dependency trees (but acylic).

License
-------

The SCons tools are licensed under the Mozilla Public License v2.0
unless otherwise noted.

