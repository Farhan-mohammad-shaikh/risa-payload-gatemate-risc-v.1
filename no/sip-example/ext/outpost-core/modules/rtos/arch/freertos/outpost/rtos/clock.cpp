/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2013, Norbert Toth
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "clock.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

outpost::time::SpacecraftElapsedTime
outpost::rtos::SystemClock::now() const
{
    // TODO Check when this will overflow
    uint64_t ticks_since_boot = static_cast<uint64_t>(xTaskGetTickCount());

    // convert to microseconds
    uint64_t us = (static_cast<uint64_t>(ticks_since_boot) * 1000000) / configTICK_RATE_HZ;

    return outpost::time::SpacecraftElapsedTime::afterEpoch(outpost::time::Microseconds(us));
}
