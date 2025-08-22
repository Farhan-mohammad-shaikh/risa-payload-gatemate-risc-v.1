<img src="doc/images/OUTPOST_logo.png" height="200px" />

The **OUTPOST** library (Open modUlar sofTware PlatfOrm for SpacecrafT)
is developed at the [German Aerospace Center (DLR)][] and provides an
execution platform targeted at embedded systems running mission
critical software.
The library set up to be modular, allowing the user to choose which
modules to use and which modules to leave out.

OUTPOST is split into two repositories: *outpost-core* contains the open-source
base environment and *outpost-satellite* contains the closed-source satellite
specific modules.


Modules
-------

This repository contains the following software modules:
- `base`

    A module for avoiding circular dependencies, includes Slice and 
    Fixedpoint as well as range traits for enums.

- `comm`

    An implementation of the RMAP protocol.

- `compression`

    Lossy and lossless compression as well as encoding of time-series data using Wavelet compression (similar to JPEG2000)

- `hal`

    Hardware abstraction layer for communication interfaces.

- `parameter`

    Implementation of a wrapper for on-board parameters (values) and corresponding stores that allow fine-grain access using references and unique parameter IDs access.

- `rtos`

    RTOS wrapper layer. Allows to build OUTPOST on different operating
    system. Supports RTEMS, FreeRTOS and all POSIX compatible OS.

- `sip`

    Simple Interface Protocol. A simple protocol to use on top of various
    physical layers, including UART, UDP and SpaceWire. Also holds
    communication pritimives for SIP-based request-response communication.

- `smpc`

    Simple Message Passing Channel. Provides a very basic, but fast
    publish/subscribe based communication middleware for objects living in
    the same address space.

- `support`

    Heartbeats can be used as a per-thread software watchdog.

- `swb`

    The software bus is an asynchronous communication paradigm allowing N:M
    communication, filtering on incoming messages and zero-copy transmit operations.

- `time`

    Type safe time management functions. Provides classes to convert
    between time representations, e.g. Unix Time to GPS or TAI.

- `utils`

    A collection of useful bits and pieces. The reuseable unit
    test harness is located here.


Installation
------------

The installation of the tool required for the library development
is described [in the INSTALL file](INSTALL.md).


License
-------

Unless otherwise noted the source code is licensed under the
terms of the *Mozilla Public License, v. 2.0* (see mpl-v2.0.txt).

Files with a different license:

 - ./ext/rapidcheck/* (BSD 2-clause)
 - ./ext/googletest-1.12.1-fused/* (BSD 3-clause)
 - ./ext/backward-cpp/* (MIT)


[German Aerospace Center (DLR)]: http://www.dlr.de/irs/en/
