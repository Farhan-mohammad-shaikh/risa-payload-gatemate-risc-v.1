/*
 * Copyright (c) 2018, Fabian Greif
 * Copyright (c) 2018, Jan Sommer
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TIME_QUOTA_H
#define OUTPOST_TIME_QUOTA_H

#include <outpost/time/clock.h>
#include <outpost/time/duration.h>
#include <outpost/time/time_epoch.h>

#include <stddef.h>

namespace outpost
{
namespace time
{
/**
 * Quota interface.
 */
class Quota
{
public:
    virtual ~Quota() = default;

    virtual void
    setTimeInterval(outpost::time::Duration interval) = 0;

    /**
     * Returns true if access was granted.
     */
    virtual bool
    access(outpost::time::SpacecraftElapsedTime now) = 0;

    /**
     * Reset status to allow access again.
     */
    virtual void
    reset() = 0;
};

/**
 * Continuous interval based quota limit.
 *
 * Allows up to \p Resources number of access during a time interval. Every
 * access afterwards is denied until at least one access was earlier than
 * the current time minus the interval.
 *
 * Uses a ring-buffer to store all access times.
 *
 * \author  Fabian Greif
 */
template <size_t Resources>
class ContinuousIntervalQuota : public Quota
{
public:
    explicit ContinuousIntervalQuota(outpost::time::Duration interval);

    void
    setTimeInterval(outpost::time::Duration interval) override;

    /**
     * Returns true if access was granted and stores the current time
     * as the last access time.
     */
    bool
    access(outpost::time::SpacecraftElapsedTime now) override;

    /**
     * Reset all stored accesses to the current time minus the current interval
     * to free up all resources.
     */
    void
    reset() override;

private:
    outpost::time::Duration mInterval;

    /// Current position within the resources ring-buffer.
    size_t mCurrentIndex;

    /// Used to store the access times in a ring-buffer like fashion
    outpost::time::SpacecraftElapsedTime mResources[Resources];
};

/**
 * Slim quota implementation with non-deterministic intervals.
 *
 * A new interval is started as soon as the first access is reported after
 * finishing the previous interval.
 */
class NonDeterministicIntervalQuota : public Quota
{
public:
    NonDeterministicIntervalQuota(outpost::time::Duration interval, size_t numberOfResources);

    void
    setTimeInterval(outpost::time::Duration interval) override;

    bool
    access(outpost::time::SpacecraftElapsedTime now) override;

    void
    reset() override;

private:
    outpost::time::Duration mInterval;
    outpost::time::SpacecraftElapsedTime mIntervalEnd;

    const size_t mNumberOfResources;
    size_t mUsedResources = 0;
};

/**
 * Quota which will always return true when asked
 */
class UnlimitedQuota : public Quota
{
public:
    UnlimitedQuota() = default;

    void
    setTimeInterval(outpost::time::Duration /*interval*/) final
    {
    }

    bool
    access(outpost::time::SpacecraftElapsedTime /*now*/) final
    {
        return true;
    }

    void
    reset() final
    {
    }
};

extern UnlimitedQuota DefaultQuota;

}  // namespace time
}  // namespace outpost

#include "quota_impl.h"

#endif
