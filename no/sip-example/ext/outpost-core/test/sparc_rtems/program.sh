#!/bin/bash
#
# Copyright (c) 2013-2017, Fabian Greif
#
# This file is part of the Simple Interface Protocols (SIP) examples.
#
# It is supplied solely for the use by TUHH and HAW Hamburg
# in the frame of the PLUTO 1 flight mission.
# Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
# Contact jan-gerd.mess@dlr.de when in doubt.

TMPDIR=".grmon"
BAUD=460800

if [ -e $GRMON ]; then
GRMON="/opt/grmon-eval/bin/grmon"
fi

if [ -z $2 ]; then
PORT="/dev/outpost_dsu_3"
else
PORT=$2
fi

mkdir -p $TMPDIR
echo "load $1
run
exit" > $TMPDIR/program.cfg
echo $GRMON -uart $PORT -stack 0x40ffffff -baud $BAUD -c $TMPDIR/program.cfg
$GRMON -uart $PORT -stack 0x40ffffff -baud $BAUD -c $TMPDIR/program.cfg
rm -rf $TMPDIR
