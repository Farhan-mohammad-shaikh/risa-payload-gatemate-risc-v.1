/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "timer.h"

void
outpost::rtos::Timer::start(time::Duration duration)
{
    (void) duration;
}

void
outpost::rtos::Timer::reset()
{
}

void
outpost::rtos::Timer::cancel()
{
}

bool
outpost::rtos::Timer::isRunning()
{
    return false;
}

void
outpost::rtos::Timer::createTimer(const char* name)
{
    (void) name;
}
