/*
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "outpost/rtos/clock.h"

#include "outpost/posix/clock.h"

using namespace outpost::rtos;

// ---------------------------------------------------------------------------
SystemClock::SystemClock() : mImplementation(nullptr)
{
    auto* clock = new outpost::posix::SystemClock();
    mImplementation = clock;
}

SystemClock::~SystemClock()
{
    auto* clock = reinterpret_cast<outpost::posix::SystemClock*>(mImplementation);
    delete clock;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
outpost::time::SpacecraftElapsedTime
SystemClock::now() const
{
    auto* clock = reinterpret_cast<outpost::posix::SystemClock*>(mImplementation);
    return clock->now();
}
