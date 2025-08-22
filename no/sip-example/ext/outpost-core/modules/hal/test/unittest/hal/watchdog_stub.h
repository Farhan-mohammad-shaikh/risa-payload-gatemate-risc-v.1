/*
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_HAL_WATCHDOG_STUB_H_
#define UNITTEST_HAL_WATCHDOG_STUB_H_

#include <outpost/hal/watchdog.h>
#include <outpost/time/clock.h>

#include <stdlib.h>

namespace unittest
{
namespace hal
{
template <size_t intervalInSeconds>
class WatchdogStub : public outpost::hal::Watchdog
{
public:
    static constexpr outpost::time::Duration interval = outpost::time::Seconds(intervalInSeconds);

    explicit WatchdogStub(outpost::time::Clock& clock) :
        mClock(clock),
        mLastReset(clock.now()),
        mFailed(false)

                {};

    bool
    reset() override
    {
        if (mClock.now() >= mLastReset + interval)
        {
            mFailed = true;
        }
        mLastReset = mClock.now();
        return true;
    }

    outpost::time::Duration
    getInterval() override
    {
        return interval;
    }

    bool
    hasFailed() const
    {
        return mFailed || mClock.now() >= mLastReset + interval;
    }

private:
    outpost::time::Clock& mClock;
    outpost::time::SpacecraftElapsedTime mLastReset;
    bool mFailed;
};

template <size_t intervalInSeconds>
constexpr outpost::time::Duration WatchdogStub<intervalInSeconds>::interval;

}  // namespace hal
}  // namespace unittest

#endif
