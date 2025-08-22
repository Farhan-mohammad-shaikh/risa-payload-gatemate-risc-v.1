#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015, 2017-2018, Fabian Greif
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

LBUILD=lbuild

show-modules:
	$(LBUILD) discover

dependencies:
	$(LBUILD) --option=":test=False" dependencies > dep.dot
	dot -Grankdir=BT -Tpng dep.dot > dep.png
	$(LBUILD) --option=":test=True" dependencies > dep_test.dot
	dot -Grankdir=BT -Tpng dep_test.dot > dep_test.png
	xdg-open dep.png
	xdg-open dep_test.png

build-all:
	#$(RM) -r source
	$(LBUILD) --path source build --module "outpost:*"
	scons -j3 -C source

clean:
	$(RM) dep.dot
	$(RM) dep.png

.PHONY : show-modules dependencies clean
