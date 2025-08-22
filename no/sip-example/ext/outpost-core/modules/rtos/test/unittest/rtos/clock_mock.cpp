/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "unittest/rtos/clock_mock.h"

using namespace outpost::posix;
using namespace unittest::rtos;

// ---------------------------------------------------------------------------
ClockImplementation::ClockImplementation(outpost::rtos::SystemClock* clock) :
    Mockable<ClockMock, outpost::posix::SystemClock>(
            this, reinterpret_cast<intptr_t>(clock), &mSystemClock)
{
}

ClockImplementation::~ClockImplementation()
{
}

// ---------------------------------------------------------------------------
outpost::time::SpacecraftElapsedTime
ClockImplementation::now() const
{
    std::function<outpost::time::SpacecraftElapsedTime()> mockFunction =
            std::bind(&ClockMock::now, mMock);
    std::function<outpost::time::SpacecraftElapsedTime()> realFunction =
            std::bind(&SystemClock::now, &mSystemClock);
    return handleMethodCall(mMock, mockFunction, realFunction);
}
