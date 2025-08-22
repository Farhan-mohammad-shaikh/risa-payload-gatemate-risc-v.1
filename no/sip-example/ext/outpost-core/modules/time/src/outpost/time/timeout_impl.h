/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Tepe, Alexander
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "timeout.h"

namespace outpost
{
namespace time
{

constexpr Timeout::Timeout(const outpost::time::Clock& clock) :
    mStartTime(), mEndTime(), mState(State::stopped), mClock(clock)
{
}

inline Timeout::Timeout(const outpost::time::Clock& clock, const outpost::time::Duration& time) :
    mStartTime(), mEndTime(), mState(State::stopped), mClock(clock)
{
    OUTPOST_ASSERT(time >= outpost::time::Duration::zero(), "Negative timeout");
    restart(time);
}

inline void
Timeout::restart(const outpost::time::Duration& time)
{
    OUTPOST_ASSERT(time >= outpost::time::Duration::zero(), "Negative timeout");
    mStartTime = mClock.now();
    mEndTime = mStartTime + time;
    mState = State::armed;
}

constexpr void
Timeout::changeDuration(const outpost::time::Duration& time)
{
    OUTPOST_ASSERT(time >= outpost::time::Duration::zero(), "Negative timeout");
    updateState();
    mEndTime = mStartTime + time;
}

constexpr void
Timeout::stop()
{
    mState = State::stopped;
}

constexpr Timeout::State
Timeout::getState() const
{
    if ((mState == State::armed) && (mClock.now() >= mEndTime))
    {
        return State::expired;
    }
    return mState;
}

constexpr void
Timeout::updateState()
{
    if ((mState == State::armed) && (mClock.now() >= mEndTime))
    {
        mState = State::expired;
    }
}

constexpr bool
Timeout::isStopped() const
{
    return (mState == State::stopped);
}

constexpr bool
Timeout::isExpired() const
{
    OUTPOST_ASSERT(mState != State::stopped, "Timeout not running");
    return (getState() == State::expired);
}

inline bool
Timeout::isExpired(outpost::time::Duration& remaining) const
{
    OUTPOST_ASSERT(mState != State::stopped, "Timeout not running");
    outpost::time::SpacecraftElapsedTime now = mClock.now();
    bool isExpired = mState == State::armed && now >= mEndTime;
    if (isExpired || mState == State::stopped)
    {
        remaining = outpost::time::Duration::zero();
    }
    else
    {
        remaining = mEndTime - now;
    }
    return isExpired;
}

constexpr bool
Timeout::isArmed() const
{
    return (getState() == State::armed);
}

inline outpost::time::Duration
Timeout::getRemainingTime() const
{
    return std::max(outpost::time::Duration::zero(), mEndTime - mClock.now());
}

}  // namespace time
}  // namespace outpost
