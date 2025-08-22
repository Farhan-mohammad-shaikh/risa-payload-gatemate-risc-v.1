/*
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "watchdog_stub.h"

#include <unittest/time/testing_clock.h>

// Just to have somewhere where that one is instantiated as otherwise
// syntax error might be missed
static unittest::time::TestingClock clk;
static unittest::hal::WatchdogStub<10> watchdog(clk);
