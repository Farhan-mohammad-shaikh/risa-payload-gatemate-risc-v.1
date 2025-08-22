# Copyright (c) 2024, Jan-Gerd Mess
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

all: build-coordinator build-worker build-uploader

build-coordinator:
	@$(MAKE) -C sip-coordinator build

build-uploader:
	@$(MAKE) -C sip-uploader build

build-worker:
	@$(MAKE) -C sip-worker build


.PHONY : sip-coordinator sip-worker
