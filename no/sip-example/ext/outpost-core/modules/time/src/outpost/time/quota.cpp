/*
 * Copyright (c) 2018, Fabian Greif
 * Copyright (c) 2018, Jan Sommer
 * Copyright (c) 2022, Tobias Pfeffer
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "quota.h"

namespace outpost
{
namespace time
{
UnlimitedQuota DefaultQuota;

NonDeterministicIntervalQuota::NonDeterministicIntervalQuota(outpost::time::Duration interval,
                                                             size_t numberOfResources) :
    mInterval(interval),
    mIntervalEnd(outpost::time::SpacecraftElapsedTime::startOfEpoch()),
    mNumberOfResources(numberOfResources)
{
}

void
NonDeterministicIntervalQuota::setTimeInterval(outpost::time::Duration interval)
{
    mInterval = interval;
}

bool
NonDeterministicIntervalQuota::access(outpost::time::SpacecraftElapsedTime now)
{
    if (now >= mIntervalEnd)
    {
        mUsedResources = 0;
        mIntervalEnd = now + mInterval;
    }

    bool granted = false;
    if (mUsedResources < mNumberOfResources)
    {
        mUsedResources++;
        granted = true;
    }
    return granted;
}

void
NonDeterministicIntervalQuota::reset()
{
    mIntervalEnd = outpost::time::SpacecraftElapsedTime::startOfEpoch();
}

}  // namespace time
}  // namespace outpost
