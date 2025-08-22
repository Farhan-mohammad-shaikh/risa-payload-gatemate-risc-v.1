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

#ifndef OUTPOST_TIME_CLOCK_H
#define OUTPOST_TIME_CLOCK_H

#include "time_point.h"

namespace outpost
{
namespace time
{
/**
 * Clock interface.
 *
 * \author  Fabian Greif
 */
class Clock
{
public:
    Clock() = default;

    virtual ~Clock() = default;

    Clock(const Clock&) = delete;

    Clock&
    operator=(const Clock&) = delete;

    virtual SpacecraftElapsedTime
    now() const = 0;
};

}  // namespace time
}  // namespace outpost

#endif
