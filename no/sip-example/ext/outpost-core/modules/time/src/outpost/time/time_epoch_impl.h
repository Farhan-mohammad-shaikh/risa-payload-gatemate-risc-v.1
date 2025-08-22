/*
 * Copyright (c) 2015-2017, Fabian Greif
 * Copyright (c) 2023-2024, Janosch Reinking
 * Copyright (c) 2024, Passenberg, Felix Constantin
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TIME_TIME_EPOCH_IMPL_H
#define OUTPOST_TIME_TIME_EPOCH_IMPL_H

#include "time_epoch.h"

#include <outpost/base/slice.h>

#include <atomic>

namespace outpost
{
namespace time
{

// ----------------------------------------------------------------------------
template <>
class TimeEpochConverter<SpacecraftElapsedTimeEpoch, GpsEpoch>
{
public:
    // offset to gps time. @warning Might jump forward / backward!
    /// \note std::atomic<Duration> is not always look free!
    static std::atomic<Duration> offsetToGpsTime;

    static TimePoint<GpsEpoch>
    convert(const TimePoint<SpacecraftElapsedTimeEpoch>& from)
    {
        return TimePoint<GpsEpoch>::afterEpoch(from.timeSinceEpoch()
                                               + offsetToGpsTime.load(std::memory_order_relaxed));
    }

    static void
    setOffset(const Duration& duration)
    {
        offsetToGpsTime.store(duration, std::memory_order_relaxed);
    }

    /**
     * Set the offset so that the given SCET equals the GPS time.
     */
    static void
    setOffset(const TimePoint<SpacecraftElapsedTimeEpoch>& scet, const TimePoint<GpsEpoch>& gps)
    {
        offsetToGpsTime.store(gps.timeSinceEpoch() - scet.timeSinceEpoch(),
                              std::memory_order_relaxed);
    }
};

template <>
class TimeEpochConverter<GpsEpoch, SpacecraftElapsedTimeEpoch>
{
public:
    static TimePoint<SpacecraftElapsedTimeEpoch>
    convert(const TimePoint<GpsEpoch>& from)
    {
        return TimePoint<SpacecraftElapsedTimeEpoch>::afterEpoch(
                from.timeSinceEpoch()
                - TimeEpochConverter<SpacecraftElapsedTimeEpoch, GpsEpoch>::offsetToGpsTime.load(
                        std::memory_order_relaxed));
    }
};

// ----------------------------------------------------------------------------
template <>
class TimeEpochConverter<TaiEpoch, UnixEpoch>
{
public:
    static constexpr int64_t offsetDaysFromTaiToUnix = 4383;
    static constexpr int64_t leapSecondsAtUnixEpoch = 10;

    static constexpr int64_t initialOffsetInSeconds =
            offsetDaysFromTaiToUnix * Duration::secondsPerDay + leapSecondsAtUnixEpoch;

    struct LeapSecondCorrection
    {
        enum Type
        {
            remove,
            add
        };
    };

    /**
     * Leap second correction table.
     *
     * When new leap seconds are introduces they have to be added here. The values
     * are calculated as follows:
     *
     *     (DateUtils::getDay(Date { 1981, 6, 30, 0, 0, 0 })
     *    - DateUtils::getDay(Date { 1958, 1,  1, 0, 0, 0 }) + 1) * 86400
     *    + number of leap seconds
     *
     * See also http://hpiers.obspm.fr/eop-pc/earthor/utc/TAI-UTC_tab.html
     */
    static constexpr int64_t leapSecondArray[] = {
            1861920036,  // 36: 2016-12-31T23:59:60Z
            1814400035,  // 35: 2015-06-30T23:59:60Z
            1719792034,  // 34: 2012-06-30T23:59:60Z
            1609459233,  // 33: 2008-12-31T23:59:60Z
            1514764832,  // 32: 2005-12-31T23:59:60Z
            1293840031,  // 31: 1998-12-31T23:59:60Z
            1246406430,  // 30: 1997-06-30T23:59:60Z
            1199145629,  // 29: 1995-12-31T23:59:60Z
            1151712028,  // 28: 1994-06-30T23:59:60Z
            1120176027,  // 27: 1993-06-30T23:59:60Z
            1088640026,  // 26: 1992-06-30T23:59:60Z
            1041379225,  // 25: 1990-12-31T23:59:60Z
            1009843224,  // 24: 1989-12-31T23:59:60Z
            946684823,   // 23: 1987-12-31T23:59:60Z
            867715222,   // 22: 1985-06-30T23:59:60Z
            804556821,   // 21: 1983-06-30T23:59:60Z
            773020820,   // 20: 1982-06-30T23:59:60Z
            741484819,   // 19: 1981-06-30T23:59:60Z
            694224018,   // 18: 1979-12-31T23:59:60Z
            662688017,   // 17: 1978-12-31T23:59:60Z
            631152016,   // 16: 1977-12-31T23:59:60Z
            599616015,   // 15: 1976-12-31T23:59:60Z
            567993614,   // 14: 1975-12-31T23:59:60Z
            536457613,   // 13: 1974-12-31T23:59:60Z
            504921612,   // 12: 1973-12-31T23:59:60Z
            473385611,   // 11: 1972-12-31T23:59:60Z
            457488010,   // 10: 1972-06-30T23:59:60Z
                         // Start of leap second correction
    };

    static constexpr int64_t
    getCorrectionFactorForLeapSeconds(const int64_t& seconds,
                                      const LeapSecondCorrection::Type& correction)
    {
        int64_t correctionFactor = 0;
        auto secondsAccumulator = seconds;
        auto leapSeconds = outpost::asSlice(leapSecondArray);
        for (size_t i = 0; (i < leapSeconds.getNumberOfElements()) && (correctionFactor == 0); ++i)
        {
            if (seconds >= leapSeconds[i])
            {
                correctionFactor = leapSeconds.getNumberOfElements() - i;

                // As leap seconds are accumulated, it can happen that adding
                // leap seconds causes the resulting time to overflow into the
                // next leap second.
                //
                // This is not possible for the latest leap seconds (i = 0),
                // therefore it is excluded here.
                if ((correction == LeapSecondCorrection::add) && (i != 0U))
                {
                    secondsAccumulator = secondsAccumulator + correctionFactor;
                    if (secondsAccumulator >= leapSeconds[i - 1U])
                    {
                        correctionFactor += 1;
                    }
                }
            }
        }

        return correctionFactor;
    }

    static constexpr TimePoint<UnixEpoch>
    convert(const TimePoint<TaiEpoch>& from)
    {
        // leap second correction
        const auto correction = getCorrectionFactorForLeapSeconds(from.timeSinceEpoch().seconds(),
                                                                  LeapSecondCorrection::remove);
        return TimePoint<UnixEpoch>::afterEpoch(from.timeSinceEpoch()
                                                - Seconds(initialOffsetInSeconds + correction));
    }
};

template <>
class TimeEpochConverter<UnixEpoch, TaiEpoch>
{
public:
    static constexpr int64_t initialOffsetInSeconds =
            TimeEpochConverter<TaiEpoch, UnixEpoch>::initialOffsetInSeconds;

    static constexpr TimePoint<TaiEpoch>
    convert(const TimePoint<UnixEpoch>& from)
    {
        const int64_t seconds = from.timeSinceEpoch().seconds() + initialOffsetInSeconds;

        using Other = TimeEpochConverter<TaiEpoch, UnixEpoch>;

        // leap second correction
        int64_t correction =
                Other::getCorrectionFactorForLeapSeconds(seconds, Other::LeapSecondCorrection::add);
        return TimePoint<TaiEpoch>::afterEpoch(from.timeSinceEpoch()
                                               + Seconds(initialOffsetInSeconds + correction));
    }
};

// ----------------------------------------------------------------------------
template <>
class TimeEpochConverter<GpsEpoch, TaiEpoch>
{
public:
    static constexpr int64_t offsetDaysTaiToGps = 8040;
    static constexpr int64_t offsetLeapSecondsTaiToGps = 19;

    static constexpr int64_t offsetInSeconds =
            offsetDaysTaiToGps * Duration::secondsPerDay + offsetLeapSecondsTaiToGps;

    static constexpr TimePoint<TaiEpoch>
    convert(const TimePoint<GpsEpoch>& from)
    {
        return TimePoint<TaiEpoch>::afterEpoch(from.timeSinceEpoch() + Seconds(offsetInSeconds));
    }
};

template <>
class TimeEpochConverter<TaiEpoch, GpsEpoch>
{
public:
    static constexpr int64_t offsetInSeconds =
            TimeEpochConverter<GpsEpoch, TaiEpoch>::offsetInSeconds;

    static constexpr TimePoint<GpsEpoch>
    convert(const TimePoint<TaiEpoch>& from)
    {
        return TimePoint<GpsEpoch>::afterEpoch(from.timeSinceEpoch() - Seconds(offsetInSeconds));
    }
};

// ----------------------------------------------------------------------------
// this allows to convert from a time point to the same epoch.
// Useful in templates to avoid special handling.
template <typename From>
class TimeEpochConverter<From, From>
{
public:
    static constexpr outpost::time::TimePoint<From>
    convert(const outpost::time::TimePoint<From>& from)
    {
        return from;
    }
};

template <>
class TimeEpochConverter<GpsEpoch, UnixEpoch>
{
public:
    static constexpr TimePoint<UnixEpoch>
    convert(const TimePoint<GpsEpoch>& from)
    {
        return from.convertTo<TimePoint<TaiEpoch>>().convertTo<TimePoint<UnixEpoch>>();
    }
};

template <>
class TimeEpochConverter<UnixEpoch, GpsEpoch>
{
public:
    static constexpr TimePoint<GpsEpoch>
    convert(const TimePoint<UnixEpoch>& from)
    {
        return from.convertTo<TimePoint<TaiEpoch>>().convertTo<TimePoint<GpsEpoch>>();
    }
};

template <>
class TimeEpochConverter<J2000Epoch, UnixEpoch>
{
public:
    static constexpr Duration offsetUnixtoJ2000 =
            outpost::time::Seconds(946727935) + outpost::time::Milliseconds(816);

    static constexpr TimePoint<UnixEpoch>
    convert(const TimePoint<J2000Epoch>& from)
    {
        return TimePoint<UnixEpoch>::afterEpoch(from.timeSinceEpoch() + offsetUnixtoJ2000);
    }
};

template <>
class TimeEpochConverter<UnixEpoch, J2000Epoch>
{
public:
    static constexpr TimePoint<J2000Epoch>
    convert(const TimePoint<UnixEpoch>& from)
    {
        return TimePoint<J2000Epoch>::afterEpoch(
                from.timeSinceEpoch()
                - TimeEpochConverter<J2000Epoch, UnixEpoch>::offsetUnixtoJ2000);
    }
};

}  // namespace time
}  // namespace outpost

#endif
