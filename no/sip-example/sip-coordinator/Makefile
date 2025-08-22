# Copyright (c) 2024, Jan-Gerd Mess
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

PROGRAM_NAME = sip_coordinator

build:
	@scons -j4 build target=host program_name=$(PROGRAM_NAME)

run: build
	$(ROOTPATH)/bin/$(PROGRAM_NAME)
