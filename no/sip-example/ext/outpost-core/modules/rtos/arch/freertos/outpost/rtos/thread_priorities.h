/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_FREERTOS_THREAD_PRIORITIES_H
#define OUTPOST_FREERTOS_THREAD_PRIORITIES_H

#include <stdint.h>

#include <cstddef>

// FreeRTOS supports priorities between 0 and (configMAX_PRIORITIES - 1).
// Lower values represent a lower priority, 0 is the priority of the idle task
// and the overall lowest priority.
//
static inline uint8_t
toFreeRtosPriority(uint8_t priority, size_t numPriorities)
{
    if (numPriorities == 0)
    {
        return 0;
    }

    const uint8_t stepWidth = 255 / numPriorities;

    uint8_t out = (priority / stepWidth);

    if (out > (numPriorities - 1))
    {
        return (numPriorities - 1);
    }
    else
    {
        return out;
    }
}

static inline uint8_t
fromFreeRtosPriority(uint8_t priority, size_t numPriorities)
{
    if (numPriorities == 0)
    {
        return 0;
    }

    const uint8_t stepWidth = 255 / numPriorities;
    const uint8_t offset = stepWidth / 2;

    return (priority * stepWidth + offset);
}

#endif
