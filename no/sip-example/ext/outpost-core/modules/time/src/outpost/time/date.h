/*
 * Copyright (c) 2015-2017, Fabian Greif
 * Copyright (c) 2024, Felix Passenberg
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

#ifndef OUTPOST_TIME_DATE_H
#define OUTPOST_TIME_DATE_H

#include "time_epoch.h"

namespace outpost
{
namespace time
{
// forward declaration
struct Date;

/**
 * Helper class to simplify the calculation of dates.
 *
 * Based on the following algorithm:
 * https://alcor.concordia.ca/~gpkatch/gdate-method.html
 */
struct DateUtils
{
    static constexpr int64_t
    getDay(Date date);

    static constexpr Date
    getDate(int64_t day);
};

/**
 * Represents a specific instant in time.
 *
 * The date is given in UTC and the conversion functions accommodates for leap
 * seconds in the time between January 1980 and ~2016 (last leap second
 * which is incorporated is from June 2015).
 *
 * \author Fabian Greif
 */
struct Date
{
    constexpr Date() : year(0), month(0), day(0), hour(0), minute(0), second(0)
    {
    }

    constexpr Date(uint16_t _year,
                   uint8_t _month,
                   uint8_t _day,
                   uint8_t _hour,
                   uint8_t _minute,
                   uint8_t _second) :
        year(_year), month(_month), day(_day), hour(_hour), minute(_minute), second(_second)
    {
    }

    static constexpr Date
    fromUnixTime(const UnixTime& time);

    static constexpr UnixTime
    toUnixTime(const Date& date);

    /**
     * Check that given value represent a valid date.
     */
    constexpr bool
    isValid() const;

    constexpr bool
    operator==(const Date& other) const;

    constexpr bool
    operator!=(const Date& other) const;

    constexpr bool
    operator<(const Date& other) const;

    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

/**
 * GPS time representation as week and seconds within that week.
 */
struct GpsDate
{
    constexpr GpsDate() : weekNumber(0), timeOfWeek(0)
    {
    }

    constexpr GpsDate(uint16_t week, uint32_t secondsInWeek) :
        weekNumber(week), timeOfWeek(secondsInWeek)
    {
    }

    static constexpr GpsDate
    fromGpsTime(const GpsTime& time);

    static constexpr GpsTime
    toGpsTime(const GpsDate& date);

    // Week number (WN)
    uint16_t weekNumber;

    /**
     * Time Of Week (TOW)
     *
     * Number of seconds within the week. Range 0..604800.
     */
    uint32_t timeOfWeek;
};

/**
 * MJD time representation as days and fractional days since 1858-11-17T00:00:00 TT
 * note not UTC, as it does not count leap seconds. it uses TT = TAI seconds.
 */
struct MjDate
{
    // TAI to TT conversion: TAI + 32.184s = TT
    // http://www.iausofa.org/sofa_ts_c.pdf
    constexpr static const Duration taiToTtAdd = Milliseconds(32184);
    // 36204.0 = MJD at 1958-01-01T00:00:00 TT
    constexpr static const int32_t mjdDayAtTaiEpoch = 36204;

    static constexpr MjDate
    fromTai(AtomicTime const& time)
    {
        // convert from TAI to TT
        const Duration terrestrialTime = time.timeSinceEpoch() + MjDate::taiToTtAdd;

        MjDate date;

        // calculate the day
        date.day = terrestrialTime.microseconds() / Duration::microsecondsPerDay
                   + MjDate::mjdDayAtTaiEpoch;

        // calculate the fractional day
        // (overall micro seconds - microseconds of day) / microseconds per day
        date.fractionalDay = static_cast<double>((terrestrialTime.microseconds()
                                                  - (date.day - MjDate::mjdDayAtTaiEpoch)
                                                            * Duration::microsecondsPerDay))
                             / Duration::microsecondsPerDay;

        return date;
    };

    static constexpr AtomicTime
    toTaiTime(MjDate const& date)
    {
        // at first the day as integer calculation
        int64_t microseconds = static_cast<int64_t>(date.day - MjDate::mjdDayAtTaiEpoch)
                               * Duration::microsecondsPerDay;
        // add the fractional part from double calculation
        microseconds += (date.fractionalDay * Duration::microsecondsPerDay);
        // finally the offset from TT to TAI
        microseconds -= MjDate::taiToTtAdd.microseconds();

        return AtomicTime::afterEpoch(Microseconds(microseconds));
    };

    int64_t day = 0;
    double fractionalDay = 0;
};

}  // namespace time
}  // namespace outpost

#include "date_impl.h"

#endif
