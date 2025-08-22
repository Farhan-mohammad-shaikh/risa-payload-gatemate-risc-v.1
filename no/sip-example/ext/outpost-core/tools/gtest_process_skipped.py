#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016-2017, Fabian Greif
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

# Jenkins xUnit processor requires that a skipped test contains
# the <skipped/> element and not just setting status to "notrun".
# This script adds the skipped tag for those tests.

import os
import sys
import re

notrun_filter = re.compile('(\s*<testcase[^<]+status="notrun"[^<]+)/>')

with open(sys.argv[2], 'w+') as output:
	with open(sys.argv[1], 'r') as input:
		for line in input:
			
			match = notrun_filter.match(line)
			if match:
				testcase = match.group(1)
				output.write(testcase + "><skipped /></testcase>\n")
			else:
				output.write(line)
