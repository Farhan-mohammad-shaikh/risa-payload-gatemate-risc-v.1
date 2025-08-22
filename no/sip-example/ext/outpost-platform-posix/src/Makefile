# Copyright (c) 2021, Jan-Gerd Mess
# Copyright (c) 2023, Mess, Jan-Gerd
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

CLANG_FORMAT=clang-format

SOURCE_FILES = $(shell find . -type f -name '*.cpp' 2>/dev/null)
HEADER_FILES = $(shell find . -type f -name '*.h' 2>/dev/null)

format:
	@$(CLANG_FORMAT) -i $(SOURCE_FILES)
	@$(CLANG_FORMAT) -i $(HEADER_FILES)