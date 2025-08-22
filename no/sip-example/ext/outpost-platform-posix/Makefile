# Copyright (c) 2017, Fabian Greif
# Copyright (c) 2021, 2023, Jan-Gerd Mess
# Copyright (c) 2023-2024, Janosch Reinking
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

# Check if the terminal supports colors
COLORS := $(shell tput colors 2> /dev/null)

ifeq ($(COLORS),)
# The terminal doesn't support colors, don't output anything
CINFO  =
COK    =
CERROR =
CEND   =
else
# Color definitions (e.g. for bash 'tput colors' returns '8')
CINFO  = \033[;0;33m
COK    = \033[32;01m
CERROR = \033[31;01m
CEND   = \033[0m
endif

BUILDPATH = build

test: build run

DIRS_FORMATTING = src test
format:
	@for m in $(DIRS_FORMATTING); do \
		printf "$(CINFO)Format directory \"$$m\"$(CEND)\n" ; \
		make -C $$m format --no-print-directory || return 1 ; \
	done
	@printf "$(COK)[PASS] Formatting done!$(CEND)\n"

build:
	scons -C . use_test_asserts=1 $(MAKEJOBS)     # build parallel

example:
	@scons -j3 -C examples/can

run:
	scons -C . use_test_asserts=1 run-unittests   # run sequentially
	@printf "\n$(COK)[PASS] All unit tests passed!$(CEND)\n"

clean:
	scons -C . -c

.PHONY : build example run clean
