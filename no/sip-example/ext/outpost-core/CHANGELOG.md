# Changelog
## 2024.2.0
- Updated and improved SCons build system with `libdep` scheme
## 2024.1.0
- [HAL] Introduction of generic GPIO interface
- [BASE] Fixpoint renamed to Fixedpoint
- [UTILS] Generalized Ring Buffer implementations to use Allocators
- [UTILS] CRC allows flexible polynomial and initial value
- [CI] Moved to GitLab CI
- [SIP] Allow for synchrnonization at arbitrary points in the stream
- [SIP] Recovery from misaligned transmission
- [UTILS] Introduction of outpost::Expected
- [RTOS] Added empty() method to queue
- [RTOS] SMP support through processor/ core affinity
- [RTOS] Implementation of interrupt-specific functions for RTEMS
- [PARAMETER] Fixed issues regarding concurrent access
- [CI] Moved main scripts to their own repository to remove redundancy
- [TESTING] OUTPOST_ASSERT macro for static assertions in tests
- Smaller Fixes
  - Slices allows copying of non-byte types
  - Cppcheck handling of suppressions improved
  - Zero-length handling of little endian serialization
  - Added time conversion to the same epoch
## 2023.2.0
- [RTOS] Fixed semaphore clean up in POSIX
- [LBUILD] Fixed lbuild support
- [SIP] Introduced parameters for protocol customization
## 2023.1.0
- Dropped 'v' in Release Tags
- [CPPCHECK] Defects mitigated
- [SCONS] Added incremental building through variant dirs
- [HAL] GPIO abstraction
- [HAL] CAN interface is now restricted to a few mandatory functions
- [EXT] Icecream library added for development testing
- [RTOS] Durations of timeouts can now be updated
- Smaller fixes:
  - [HAL] Filesystem uses const char*
  - [UTILS] Bug in Bigendian Integers fixed
  - [UTILS] Behaviour of SharedBufferPointer.getCHild more consistent
  - [UTILS] Warning in dequeue removed
## v2022.4.1
- [SMPC] Added dummy parameter for functor call to avoid compiler warnings.
## v2022.4.0
- [UTILS] Rework of functors to return an operation result on registration and function calling
- [SCons] Moved SConscript.library and SConscript.test to root folder.
- [BASE] Added a copyFrom() method to Slices for safer copying
- Several erros and warning stemming from static analysis (Polyspace) have been fixed
## v2022.3.0
- [EXT] GTest updated to 1.12.1
- [UTILS] Introduction of ByteArrayRingBuffer (formerly in OUTPOST Satellite)
- [UTILS] Functors allow validity checks and execution results
- [L3TEST] Removed L3Test module