/*
 * Copyright (c) 2018, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TIME_QUOTA_IMPL_H
#define OUTPOST_TIME_QUOTA_IMPL_H

#include "quota.h"

namespace outpost
{
namespace time
{
template <size_t Resources>
ContinuousIntervalQuota<Resources>::ContinuousIntervalQuota(outpost::time::Duration interval) :
    mInterval(interval), mCurrentIndex(0)
{
    // reset, but reset() is virtual
    for (size_t i = 0; i < Resources; ++i)
    {
        // set as far back as possible
        mResources[i] = SpacecraftElapsedTime::startOfEpoch() - Duration::maximum();
    }
}

template <size_t Resources>
void
ContinuousIntervalQuota<Resources>::setTimeInterval(outpost::time::Duration interval)
{
    mInterval = interval;
}

template <size_t Resources>
bool
ContinuousIntervalQuota<Resources>::access(outpost::time::SpacecraftElapsedTime now)
{
    size_t next = (mCurrentIndex + 1) % Resources;

    bool accessGranted = false;
    outpost::time::SpacecraftElapsedTime t = mResources[next];
    if (now >= t + mInterval)
    {
        mResources[next] = now;
        mCurrentIndex = next;
        accessGranted = true;
    }

    return accessGranted;
}

template <size_t Resources>
void
ContinuousIntervalQuota<Resources>::reset()
{
    mCurrentIndex = 0;
    for (size_t i = 0; i < Resources; ++i)
    {
        // set as far back as possible
        mResources[i] = SpacecraftElapsedTime::startOfEpoch() - Duration::maximum();
    }
}

}  // namespace time
}  // namespace outpost

#endif
