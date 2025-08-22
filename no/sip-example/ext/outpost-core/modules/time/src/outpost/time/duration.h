/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2015, Annika Ofenloch
 * Copyright (c) 2016, Jan Sommer
 * Copyright (c) 2020-2021, Jan Malburg
 * Copyright (c) 2021, 2024, Felix Passenberg
 * Copyright (c) 2021, Andre Nahrwold
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TIME_DURATION_H
#define OUTPOST_TIME_DURATION_H

#include <limits>
#include <stdint.h>

namespace outpost
{
namespace time
{
/**
 * A length of time unattached to any point on the time continuum.
 *
 * Available operations:
 *
 *     Duration + Duration  --> Duration
 *     Duration - Duration  --> Duration
 *
 *     Duration * Integer   --> Duration
 *     Integer  * Duration  --> Duration
 *     Duration / Integer   --> Duration  (Integer Division rules)
 *     Duration / Duration  --> Integer   (Integer Division rules)
 *     Duration % Duration  --> Duration
 *
 * Resolution is microseconds.
 *
 * When using a microsecond resolution a 32 bit value would only
 * provide a range of ~2200 seconds which is insufficient.
 *
 * With 64 bit a time span of 9 * 10^12 seconds is possible:
 * (2^63 - 1) / 1,000,000 / 60 / 60 / 24 /365 = 292,471 years
 *
 * times longer than 10,000 years may be considered to never happen (myriad)
 *
 * \ingroup time
 * \author  Fabian Greif
 */

// 64 bit can run for ~292471 years; 32 bit only to ~2200 seconds.
typedef int64_t TickType;

class Duration
{
private:
    friend constexpr Duration
    operator*(const TickType& multiplier, const Duration& duration);

    template <class T>
    friend struct std::numeric_limits;

public:
    template <typename ReferenceEpoch>
    friend class TimePoint;

    static constexpr TickType nanosecondsPerMicrosecond = 1000;
    static constexpr TickType microsecondsPerMillisecond = 1000;
    static constexpr TickType millisecondsPerSecond = 1000;
    static constexpr TickType secondsPerMinute = 60;
    static constexpr TickType minutesPerHour = 60;

    static constexpr TickType secondsPerDay = 86400;
    static constexpr TickType millisecondsPerDay = secondsPerDay * millisecondsPerSecond;
    static constexpr TickType microsecondsPerDay = millisecondsPerDay * microsecondsPerMillisecond;

    inline ~Duration() = default;

    /**
     * Default constructor
     */
    constexpr Duration() : mTicks(0)
    {
    }

    /**
     * Copy constructor
     */
    constexpr Duration(const Duration& other) = default;

    constexpr Duration&
    operator=(const Duration& other) = default;

    /**
     * Get duration in hours.
     *
     * \return  Returns number of hours truncating any fractional hours.
     */
    constexpr TickType
    hours() const
    {
        return mTicks
               / (microsecondsPerMillisecond * millisecondsPerSecond * secondsPerMinute
                  * minutesPerHour);
    }

    /**
     * Get duration in minutes.
     *
     * \return  Returns number of minutes truncating any fractional minutes.
     */
    constexpr TickType
    minutes() const
    {
        return mTicks / (microsecondsPerMillisecond * millisecondsPerSecond * secondsPerMinute);
    }

    /**
     * Get duration in seconds.
     *
     * \return  Returns number of seconds truncating any fractional seconds.
     */
    constexpr TickType
    seconds() const
    {
        return mTicks / (microsecondsPerMillisecond * millisecondsPerSecond);
    }

    /**
     * Get duration in milliseconds.
     *
     * \return    Returns number of milliseconds truncating any
     *             fractional milliseconds.
     */
    constexpr TickType
    milliseconds() const
    {
        return mTicks / microsecondsPerMillisecond;
    }

    /**
     * Get duration in microseconds.
     *
     * \return    Returns number of microseconds truncating any
     *             fractional microseconds.
     */
    constexpr TickType
    microseconds() const
    {
        return mTicks;
    }

    /**
     * the maximum value the type can hold
     *
     * for reasonably long durations better use myriad
     * they are safer against calculations.
     * maximum will overflow.
     *
     */
    static constexpr Duration
    maximum()
    {
        return Duration(std::numeric_limits<outpost::time::TickType>::max());
    }

#if __cplusplus >= 201309L
    [[deprecated("ambiguous: use myriad for long time, use numeric_limits<Duration>::max() for the "
                 "absolute maximum value")]]
#endif
    static constexpr Duration
    infinity()
    {
        return maximum();
    }

    /**
     * approximately 10000 years.
     *
     * does not calculate leap years, leap seconds and other stuff.
     * some places use it to consider the timeout as never happening.
     * Banzai!
     */
    static constexpr Duration
    myriad()  // 10000 years
    {
        return Duration(10000 * 365 * 24 * minutesPerHour * secondsPerMinute * millisecondsPerSecond
                        * microsecondsPerMillisecond);
    }

    static constexpr Duration
    zero()
    {
        return Duration(0);
    }

    constexpr Duration
    operator-() const
    {
        return Duration(-mTicks);
    }

    constexpr Duration
    operator-(const Duration& other) const
    {
        return Duration(mTicks - other.mTicks);
    }

    constexpr Duration
    operator+(const Duration& other) const
    {
        return Duration(mTicks + other.mTicks);
    }

    constexpr Duration
    operator/(const TickType& divisor) const
    {
        return Duration(mTicks / divisor);
    }

    constexpr TickType
    operator/(const Duration& divisor) const
    {
        return mTicks / divisor.mTicks;
    }

    constexpr Duration
    operator%(const Duration& divisor) const
    {
        return Duration(mTicks % divisor.mTicks);
    }

    constexpr Duration
    operator-=(const Duration& other)
    {
        mTicks = mTicks - other.mTicks;
        return Duration(mTicks);
    }

    constexpr Duration
    operator+=(const Duration& other)
    {
        mTicks = mTicks + other.mTicks;
        return Duration(mTicks);
    }

    constexpr const Duration
    operator/=(TickType divisor)
    {
        mTicks = mTicks / divisor;
        return Duration(mTicks);
    }

    constexpr const Duration
    operator*(TickType rhs) const
    {
        return Duration(mTicks * rhs);
    }

    constexpr const Duration
    operator*=(TickType divisor)
    {
        mTicks = mTicks * divisor;
        return Duration(mTicks);
    }

    constexpr bool
    operator<(const Duration& rhs) const
    {
        return mTicks < rhs.mTicks;
    }

    constexpr bool
    operator>(const Duration& rhs) const
    {
        return mTicks > rhs.mTicks;
    }

    constexpr bool
    operator<=(const Duration& rhs) const
    {
        return mTicks <= rhs.mTicks;
    }

    constexpr bool
    operator>=(const Duration& rhs) const
    {
        return mTicks >= rhs.mTicks;
    }

    constexpr bool
    operator==(const Duration& rhs) const
    {
        return mTicks == rhs.mTicks;
    }

    constexpr bool
    operator!=(const Duration& rhs) const
    {
        return mTicks != rhs.mTicks;
    }

    constexpr Duration
    abs() const
    {
        return mTicks < 0 ? Duration(-mTicks) : Duration(mTicks);
    }

protected:
    explicit constexpr Duration(const TickType& numberOfTicks) : mTicks(numberOfTicks)
    {
    }

private:
    // When using a microsecond resolution a 32 bit value would only
    // provide a range of ~2200 seconds which is insufficient.
    //
    // With 64 bit a time span of 9 * 10^12 seconds is possible:
    // (2^63 - 1) / 1,000,000 / 60 / 60 / 24 /365 = 292,471 years
    //
    // times longer than 10,000 years may be considered to never happen (myriad)
    TickType mTicks;
};

constexpr Duration
operator*(const TickType& multiplier, const Duration& duration)
{
    return Duration(duration.mTicks * multiplier);
}

class Hours : public Duration
{
public:
    explicit constexpr Hours(const TickType& value) :
        Duration(value * minutesPerHour * secondsPerMinute * millisecondsPerSecond
                 * microsecondsPerMillisecond)
    {
    }
};

class Minutes : public Duration
{
public:
    explicit constexpr Minutes(const TickType& value) :
        Duration(value * secondsPerMinute * millisecondsPerSecond * microsecondsPerMillisecond)
    {
    }
};

/**
 * Allows expression of durations as a seconds count.
 *
 * \ingroup    time
 */
class Seconds : public Duration
{
public:
    explicit constexpr Seconds(const TickType& value) :
        Duration(value * millisecondsPerSecond * microsecondsPerMillisecond)
    {
    }
};

/**
 * Allows expression of durations as a milliseconds.
 *
 * \ingroup    time
 */
class Milliseconds : public Duration
{
public:
    explicit constexpr Milliseconds(const TickType& value) :
        Duration(value * microsecondsPerMillisecond)
    {
    }
};

/**
 * Allows expression of durations as a microseconds.
 *
 * \ingroup    time
 */
class Microseconds : public Duration
{
public:
    explicit constexpr Microseconds(const TickType& value) : Duration(value)
    {
    }
};

// ---------------------------------------------------------------------------
namespace literals
{

constexpr Duration
operator"" _us(unsigned long long us)
{
    return Microseconds(us);
}

constexpr Duration
operator"" _ms(unsigned long long ms)
{
    return Milliseconds(ms);
}

constexpr Duration
operator"" _s(unsigned long long s)
{
    return Seconds(s);
}

constexpr Duration
operator"" _min(unsigned long long min)
{
    return Minutes(min);
}

constexpr Duration
operator"" _h(unsigned long long hours)
{
    return Hours(hours);
}

}  // namespace literals

using outpost::time::literals::operator""_us;
using outpost::time::literals::operator""_ms;
using outpost::time::literals::operator""_s;
using outpost::time::literals::operator""_min;
using outpost::time::literals::operator""_h;

}  // namespace time
}  // namespace outpost

namespace std
{
// only replacing where the return type was TickType, thus the rest is inherited
template <>
struct numeric_limits<outpost::time::Duration> : public numeric_limits<outpost::time::TickType>
{
    static constexpr outpost::time::Duration
    max()
    {
        return outpost::time::Duration(numeric_limits<outpost::time::TickType>::max());
    }

    static constexpr outpost::time::Duration
    min()
    {
        return outpost::time::Duration(numeric_limits<outpost::time::TickType>::min());
    }
#if __cplusplus >= 201103L
    static constexpr outpost::time::Duration
    lowest() noexcept
    {
        return min();
    }
#endif
    static constexpr outpost::time::Duration
    epsilon() noexcept
    {
        return outpost::time::Duration::zero();
    }
};
}  // namespace std

#endif
