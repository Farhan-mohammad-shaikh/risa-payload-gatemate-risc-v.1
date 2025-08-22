
# Installation

Internally at DLR, we use either a Debian- or Ubuntu-based development environment.
Please make sure your system is up to date.

## Debian

Some of the tools found on our CI machines include version that require Debian's
unstable sources. To add them, append the following lines to your `/etc/apt/sources.list`

`deb http://ftp.de.debian.org/debian/ sid main contrib non-free`
`deb-src http://ftp.de.debian.org/debian/ sid main` 

## Install dependencies for library development

The full tool-suite is only required if you want to run all unit tests and
actively develop the library.

To use the library the following tools are sufficient:

- `gcc` >= 4.9 (full support for C++11)
or
- `clang` >= 3.2
- `scons` >= 4.1

### Install build tools

```
sudo apt install gcc g++ cmake valgrind doxygen python3 python3-pip \
git clang llvm cppcheck libboost-all-dev pkg-config libsqlite3-dev \ libarchive-dev
```

### Unittests

For optionally nicely printed backtraces if `OUTPOST_ASSERT(...)` fails, libdwarf and libelf is needed.

```
sudo apt-get install libdwarf-dev libelf-dev
```

### clang-format

Install clang-format:

```
sudo apt install clang-format
```

### clang-tidy

clang-tidy should be available in the distribution repositories:

```
sudo apt-get install clang-tidy
sudo apt-get install clang-tidy-<version>
```
A symbolic link might be necessary:

```
sudo ln -s /usr/bin/run-clang-tidy-14 /usr/bin/run-clang-tidy
```

Furthermore `clang-html` is used to generate html files from the logs:

```
pip install clang-html
```

### cppcheck

Install cppcheck:

```
sudo apt install cppcheck
```

Currenty, the DLR CI uses cppcheck in version 2.11

=======

