/*
 * Copyright (c) 2014-2018, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "testing_clock.h"

using namespace unittest::time;

TestingClock::TestingClock() : currentTime(outpost::time::SpacecraftElapsedTime::startOfEpoch())
{
}

outpost::time::SpacecraftElapsedTime
TestingClock::now() const
{
    return currentTime;
}

void
TestingClock::setTime(outpost::time::SpacecraftElapsedTime timePoint)
{
    currentTime = timePoint;
}

void
TestingClock::incrementBy(outpost::time::Duration time)
{
    currentTime += time;
}
