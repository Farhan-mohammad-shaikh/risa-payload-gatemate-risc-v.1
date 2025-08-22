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

#include "unittest/rtos/clock_mock.h"

using namespace outpost::rtos;

// ---------------------------------------------------------------------------
SystemClock::SystemClock() : mImplementation(nullptr)
{
    auto* clock = new unittest::rtos::ClockImplementation(this);
    mImplementation = clock;
}

SystemClock::~SystemClock()
{
    auto* clock = reinterpret_cast<unittest::rtos::ClockImplementation*>(mImplementation);
    delete clock;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
outpost::time::SpacecraftElapsedTime
SystemClock::now() const
{
    auto* clock = reinterpret_cast<unittest::rtos::ClockImplementation*>(mImplementation);
    return clock->now();
}
