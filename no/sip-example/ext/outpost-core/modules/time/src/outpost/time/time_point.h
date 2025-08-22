/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TIME_TIME_POINT_H
#define OUTPOST_TIME_TIME_POINT_H

#include "duration.h"

namespace outpost
{
namespace time
{
// forward declaration
template <typename From, typename To>
class TimeEpochConverter;

/**
 * Specifier for a location in the time continuum.
 *
 * Describes a time relative to an absolute time. The reference absolute time
 * is implementation defined and not fixed. The class \c TimeModel is
 * responsible for conversions between UTC and this class.
 *
 * Another way of saying the same thing, is that a TimePoint represents an
 * epoch plus or minus a duration.
 *
 * Available Operations:
 *
 *     TimePoint + Duration  --> TimePoint
 *     TimePoint - Duration  --> TimePoint
 *     TimePoint - TimePoint --> Duration
 *
 * Unlike regular numeric types, the following operations are
 * undefined:
 *
 *     Duration  + TimePoint --> Undefined
 *     Duration  - TimePoint --> Undefined
 *     TimePoint + TimePoint --> Undefined
 *
 * The resolution of a TimePoint is microseconds.
 *
 * \ingroup time
 * \author  Fabian Greif
 */
template <typename ReferenceEpoch>
class TimePoint
{
public:
    typedef ReferenceEpoch Epoch;

    constexpr TimePoint() : mDuration(Duration::zero())
    {
    }

    constexpr TimePoint(const TimePoint& other) : mDuration(other.mDuration)
    {
    }

    ~TimePoint() = default;

    constexpr Duration
    operator-(const TimePoint& other) const
    {
        return Duration(mDuration - other.mDuration);
    }

    constexpr TimePoint
    operator-(const Duration& duration) const
    {
        return TimePoint(mDuration - duration);
    }

    constexpr TimePoint&
    operator-=(const Duration& d)
    {
        mDuration = mDuration - d;
        return *this;
    }

    constexpr TimePoint
    operator+(const Duration& d) const
    {
        return TimePoint(mDuration + d);
    }

    constexpr TimePoint&
    operator+=(const Duration& d)
    {
        mDuration = mDuration + d;
        return *this;
    }

    constexpr bool
    operator==(const TimePoint& other) const
    {
        return (mDuration == other.mDuration);
    }

    constexpr bool
    operator!=(const TimePoint& other) const
    {
        return (mDuration != other.mDuration);
    }

    constexpr bool
    operator<(const TimePoint& other) const
    {
        return (mDuration < other.mDuration);
    }

    constexpr bool
    operator>(const TimePoint& other) const
    {
        return (mDuration > other.mDuration);
    }

    constexpr bool
    operator<=(const TimePoint& other) const
    {
        return (mDuration <= other.mDuration);
    }

    constexpr bool
    operator>=(const TimePoint& other) const
    {
        return (mDuration >= other.mDuration);
    }

    constexpr TimePoint&
    operator=(const TimePoint& other)
    {
        // This gracefully handles self assignment
        mDuration = other.mDuration;
        return *this;
    }

    /**
     * Get the duration since the start of the epoch.
     */
    constexpr Duration
    timeSinceEpoch() const
    {
        return mDuration;
    }

    /**
     * Construct a time point a given time after the start of the epoch.
     */
    static constexpr TimePoint<ReferenceEpoch>
    afterEpoch(const Duration& duration)
    {
        return TimePoint(duration);
    }

    static constexpr TimePoint<ReferenceEpoch>
    startOfEpoch()
    {
        return TimePoint(Duration::zero());
    }

    static constexpr TimePoint<ReferenceEpoch>
    endOfEpoch()
    {
        return TimePoint(Duration::myriad());
    }

    template <typename To>
    constexpr To
    convertTo() const
    {
        return TimeEpochConverter<ReferenceEpoch, typename To::Epoch>::convert(*this);
    }

protected:
    explicit constexpr TimePoint(const Duration& duration) : mDuration(duration)
    {
    }

private:
    Duration mDuration;
};

}  // namespace time
}  // namespace outpost

#include "time_epoch.h"

#endif
