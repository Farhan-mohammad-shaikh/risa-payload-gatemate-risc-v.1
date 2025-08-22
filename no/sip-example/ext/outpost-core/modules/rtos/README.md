# Module RTOS

This module provides an abstraction layer for basic RTOS functionality like Threads, Mutex etc for various platforms. The platform must be selected at compile time by setting `envGlobal['OS'] = '<rtos>'` or `set(OUTPOST_OS "<rtos>")`. The current list of RTOS implementation are:

 - FreeRTOS
 - none (empty implementation)
 - POSIX
 - RTEMS
 - unittest

The latter can be used for testing. The module does not actually provide an RTOS implementation. Instead only headers are provide which are just good enough to compile a project which uses outposts RTOS module. Without linking any further libraries a build would yield in a `undefined reference` error. Therefore for each executable an additional library must be linked, which contains the actual implementation of the header files. This allows to have multiple implementation of the RTOS headers. Which becomes very handy when testing multi-threaded code, since the tester can chose between different implementation which suites the need of the test the most. Currently there are two implementations available:

 - POSIX, which has the same behavior as the common POSIX RTOS
 - mockable RTOS, which can be used to mock the RTOS classes

But also a custom implementation can be used. There are some examples available under `./test`. Per default the POSIX implementation is used. It can be changed by using `link_library_overrides` (scons). Be aware that this will only exchange the linked libraries. Dependencies of the new library are not automatically added.

```python
files = envGlobal.RecursiveGlob('.cpp', searchdir='outpost')
files += [envGlobal.File('main.cpp')]
library_overrides = {
    'outpost_rtos_posix': 'outpost_rtos_mock'
}

envGlobal.RegisterTest(module, files, unittest_deps,
                        link_library_overrides=library_overrides)
```

In CMake a implementation must be explicit added to the linked libraries. `outpost_add_test` will automatically add `outpost_rtos_unittest_posix` if no implementation was added to the dependencies (`deps`).

```cmake
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")

set(deps
    outpost_rtos_unittest_mock
    googletest-local
)

outpost_add_test(MODULE outpost_rtos SOURCES ${sources} DEPENDENCIES ${deps})
```