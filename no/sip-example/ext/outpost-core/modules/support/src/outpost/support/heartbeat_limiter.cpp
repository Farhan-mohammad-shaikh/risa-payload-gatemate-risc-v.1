/*
 * Copyright (c) 2018, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "heartbeat_limiter.h"

#include <outpost/parameter/support.h>

namespace outpost
{
namespace support
{
void
HeartbeatLimiter::send(outpost::time::Duration processingTimeout)
{
    outpost::time::SpacecraftElapsedTime currentTime = mClock.now();
    outpost::time::Duration timeout =
            mHeartbeatInterval + processingTimeout + parameter::heartbeatTolerance;

    // Send a new heartbeat if either the heartbeat interval has been reached
    // or if the execution timeout has been significantly reduced.
    if ((mTimeout <= (currentTime + processingTimeout + parameter::heartbeatTolerance))
        || (mTimeout > (currentTime + timeout)))
    {
        mTimeout = currentTime + timeout;
        Heartbeat::send(mSource, timeout);
    }
}

}  // namespace support
}  // namespace outpost
