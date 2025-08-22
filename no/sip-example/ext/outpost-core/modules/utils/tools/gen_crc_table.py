#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, 2017, Fabian Greif
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

import string

file_template = string.Template("""
static const uint16_t crcTable[256] = {
	$table
};

""")

def crc(value):
	tmp = 0
	if ((i & 1) != 0):
		tmp = tmp ^ 0x1021
	if ((i & 2) != 0):
		tmp = tmp ^ 0x2042
	if ((i & 4) != 0):
		tmp = tmp ^ 0x4084
	if ((i & 8) != 0):
		tmp = tmp ^ 0x8108
	if ((i & 16) != 0):
		tmp = tmp ^ 0x1231
	if ((i & 32) != 0):
		tmp = tmp ^ 0x2462
	if ((i & 64) != 0):
		tmp = tmp ^ 0x48C4
	if ((i & 128) != 0):
		tmp = tmp ^ 0x9188
	return tmp & 0xffff

if __name__ == '__main__':
	table = ""
	for i in range(256):
		table += "0x%04x, " % crc(i)
		if i % 8 == 7 and i != 255:
			table += "\n\t"

	file = open("crc_table.h", "w")
	file.write(file_template.substitute({ 'table': table }))
	file.close()

# vim: set noet ci pi sts=0 sw=2 ts=2:
