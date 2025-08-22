/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "clock.h"

outpost::time::SpacecraftElapsedTime __attribute__((weak)) outpost::rtos::SystemClock::now() const
{
    // convert to microseconds
    uint64_t us = 0;

    return outpost::time::SpacecraftElapsedTime::afterEpoch(outpost::time::Microseconds(us));
}
