/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_POSIX_CLOCK_H
#define OUTPOST_POSIX_CLOCK_H

#include <outpost/time/clock.h>
#include <outpost/time/time_point.h>

namespace outpost
{
namespace posix
{
/**
 *
 * \ingroup    rtos
 */
class SystemClock : public time::Clock
{
public:
    virtual time::SpacecraftElapsedTime
    now() const override;
};

}  // namespace posix
}  // namespace outpost

#endif
