# Copyright (c) 2015-2017, Fabian Greif
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

BOARD=5

build:
	@scons install

program: build
	@./program.sh bin/rtems.elf /dev/outpost_dsu_$(BOARD)

terminal-console:
	@picocom -b38400 /dev/outpost_console_$(BOARD)

terminal-link:
	@picocom -b38400 /dev/outpost_link_$(BOARD)

