/*
 * Copyright (c) 2015-2017, Fabian Greif
 * Copyright (c) 2024, Felix Passenberg
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2023-2024, Janosch Reinking
 * Copyright (c) 2024, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <harness/time.h>

#include <outpost/time/date.h>

#include <gmock/gmock.h>

#include <unittest/harness.h>

using namespace outpost::time;

TEST(DateTest, shouldDetectValidDates)
{
    Date date;

    date = Date{1980, 2, 28, 23, 59, 59};
    EXPECT_TRUE(date.isValid());

    date = Date{1980, 2, 29, 0, 0, 0};
    EXPECT_TRUE(date.isValid());

    date = Date{1980, 3, 1, 0, 0, 0};
    EXPECT_TRUE(date.isValid());

    date = Date{1992, 1, 1, 12, 12, 12};
    EXPECT_TRUE(date.isValid());

    date = Date{2000, 1, 1, 0, 0, 0};
    EXPECT_TRUE(date.isValid());

    date = Date{2000, 6, 1, 23, 59, 58};
    EXPECT_TRUE(date.isValid());

    date = Date{1989, 12, 31, 23, 59, 59};
    EXPECT_TRUE(date.isValid());

    date = Date{1990, 1, 1, 0, 0, 0};
    EXPECT_TRUE(date.isValid());

    date = Date{2005, 12, 31, 23, 59, 59};
    EXPECT_TRUE(date.isValid());

    date = Date{2006, 1, 1, 0, 0, 0};
    EXPECT_TRUE(date.isValid());

    date = Date{2015, 6, 30, 23, 59, 59};
    EXPECT_TRUE(date.isValid());

    date = Date{2015, 7, 1, 0, 0, 0};
    EXPECT_TRUE(date.isValid());
}

TEST(DateTest, equalComparisons)
{
    Date date = {1980, 2, 28, 23, 59, 59};
    Date sameDate = date;
    Date notSameDate = {1980, 2, 28, 23, 59, 58};

    EXPECT_TRUE(date == sameDate);
    EXPECT_FALSE(date != sameDate);

    EXPECT_FALSE(date == notSameDate);
    EXPECT_TRUE(date != notSameDate);
}

TEST(DateTest, geComparisons)
{
    Date date = {1980, 2, 28, 23, 59, 59};
    Date samedate = date;

    Date dateY = {1979, 2, 28, 23, 59, 59};
    Date dateM = {1980, 1, 28, 23, 59, 59};
    Date dateD = {1980, 2, 27, 23, 59, 59};
    Date dateH = {1980, 2, 28, 22, 59, 59};
    Date dateMin = {1980, 2, 28, 23, 58, 59};
    Date dateS = {1980, 2, 28, 23, 59, 58};

    EXPECT_FALSE(date < samedate);

    EXPECT_TRUE(dateY < date);
    EXPECT_TRUE(dateM < date);
    EXPECT_TRUE(dateD < date);
    EXPECT_TRUE(dateH < date);
    EXPECT_TRUE(dateMin < date);
    EXPECT_TRUE(dateS < date);
}

TEST(DateTest, convertDatesToUnixTime)
{
    // Calculated reference values from
    // "http://www.andrews.edu/~tzs/timeconv/timeconvert.php" test the beginning of GPS
    // seconds (06.01.1980)
    UnixTime time = Date::toUnixTime(Date{1970, 1, 1, 0, 0, 0});
    EXPECT_EQ(Seconds(0), time.timeSinceEpoch());
}

TEST(DateTest, convertDatesToUnixTimeForRandomDates)
{
    UnixTime time = Date::toUnixTime(Date{1992, 1, 1, 12, 12, 12});
    EXPECT_EQ(Seconds(694267932), time.timeSinceEpoch());

    time = Date::toUnixTime(Date{2000, 1, 1, 0, 0, 0});
    EXPECT_EQ(Seconds(946684800), time.timeSinceEpoch());

    time = Date::toUnixTime(Date{2000, 6, 1, 23, 59, 58});
    EXPECT_EQ(Seconds(959903998), time.timeSinceEpoch());
}

TEST(DateTest, convertDatesToUnixTimeForLeapYears)
{
    UnixTime time = Date::toUnixTime(Date{1980, 2, 28, 23, 59, 59});
    EXPECT_EQ(Seconds(320630399), time.timeSinceEpoch());

    time = Date::toUnixTime(Date{1980, 2, 29, 0, 0, 0});
    EXPECT_EQ(Seconds(320630400), time.timeSinceEpoch());

    time = Date::toUnixTime(Date{1980, 3, 1, 0, 0, 0});
    EXPECT_EQ(Seconds(320716800), time.timeSinceEpoch());
}

TEST(DateTest, convertDatesToUnixTimeDuringLeapSeconds)
{
    UnixTime time = Date::toUnixTime(Date{1989, 12, 31, 23, 59, 59});
    EXPECT_EQ(Seconds(631151999).seconds(), time.timeSinceEpoch().seconds());

    time = Date::toUnixTime(Date{1990, 1, 1, 0, 0, 0});
    EXPECT_EQ(Seconds(631152000), time.timeSinceEpoch());

    time = Date::toUnixTime(Date{2005, 12, 31, 23, 59, 59});
    EXPECT_EQ(Seconds(1136073599), time.timeSinceEpoch());

    time = Date::toUnixTime(Date{2006, 1, 1, 0, 0, 0});
    EXPECT_EQ(Seconds(1136073600), time.timeSinceEpoch());

    time = Date::toUnixTime(Date{2015, 6, 30, 23, 59, 59});
    EXPECT_EQ(Seconds(1435708799), time.timeSinceEpoch());

    time = Date::toUnixTime(Date{2015, 7, 1, 0, 0, 0});
    EXPECT_EQ(Seconds(1435708800), time.timeSinceEpoch());
}

TEST(DateTest, convertUnixTimeToDate)
{
    // Calculated reference values from
    // "http://www.andrews.edu/~tzs/timeconv/timeconvert.php" test the beginning of GPS
    // seconds (06.01.1980)
    Date date = Date::fromUnixTime(UnixTime::afterEpoch(Seconds(315964800)));
    Date expect{1980, 1, 6, 0, 0, 0};
    EXPECT_EQ(expect, date);
}

TEST(DateTest, convertUnixTimeToDateForRandomTimes)
{
    Date date = Date::fromUnixTime(UnixTime::afterEpoch(Seconds(694267932)));
    Date expect{1992, 1, 1, 12, 12, 12};
    EXPECT_EQ(expect, date);
}

TEST(DateTest, convertUnixTimeToDateForLeapYears)
{
    Date date = Date::fromUnixTime(UnixTime::afterEpoch(Seconds(320630399)));
    Date expect{1980, 2, 28, 23, 59, 59};
    EXPECT_EQ(expect, date);

    date = Date::fromUnixTime(UnixTime::afterEpoch(Seconds(320630400)));
    expect = Date{1980, 2, 29, 0, 0, 0};
    EXPECT_EQ(expect, date);
}

TEST(DateTest, convertUnixTimeToDateForLeapSeconds)
{
    Date date = Date::fromUnixTime(UnixTime::afterEpoch(Seconds(1435708799)));
    Date expect{2015, 6, 30, 23, 59, 59};
    EXPECT_EQ(expect, date);

    date = Date::fromUnixTime(UnixTime::afterEpoch(Seconds(1435708800)));
    expect = Date{2015, 7, 1, 0, 0, 0};
    EXPECT_EQ(expect, date);
}

TEST(DateTest, shouldDoDateRoundtripForAllValidDates)
{
    Date expect{100, 1, 1, 0, 0, 0};
    for (int year = 100; year < 3000; ++year)
    {
        for (int month = 1; month <= 12; ++month)
        {
            expect.day = 1;
            while (expect.isValid())
            {
                UnixTime time = Date::toUnixTime(expect);
                Date actual = Date::fromUnixTime(time);

                EXPECT_EQ(expect, actual);
                ++expect.day;
            }
        }
    }
}

TEST(DateTest, testMjdConstants)
{
    // check the offset is correct.
    EXPECT_EQ(MjDate::mjdDayAtTaiEpoch, 36204);
}

TEST(DateTest, testMjdAtBeginOfEpochs)
{
    MjDate date;
    date.day = 0;
    date.fractionalDay = 0;
    AtomicTime tai = AtomicTime::afterEpoch(Duration::zero());

    // ***************************
    // test at begin of TT Epoch.
    tai -= outpost::time::MjDate::taiToTtAdd;
    date = MjDate::fromTai(tai);
    EXPECT_EQ(tai.timeSinceEpoch().microseconds(),
              -outpost::time::MjDate::taiToTtAdd.microseconds());
    EXPECT_EQ(date.day, MjDate::mjdDayAtTaiEpoch);
    EXPECT_DOUBLE_EQ(date.fractionalDay, 0.0);

    // ***************************
    // test at begin of MJD Epoch.
    date.day = 0;
    date.fractionalDay = 0;

    tai = outpost::time::MjDate::toTaiTime(date);

    EXPECT_EQ(tai.timeSinceEpoch().seconds() + MjDate::taiToTtAdd.seconds(),
              static_cast<int64_t>(-36204) * Duration::secondsPerDay);
    EXPECT_EQ(tai.timeSinceEpoch().microseconds() + MjDate::taiToTtAdd.microseconds(),
              static_cast<int64_t>(-36204) * Duration::secondsPerDay
                      * Duration::millisecondsPerSecond * Duration::microsecondsPerMillisecond);
    EXPECT_EQ(date.day, 0);
    EXPECT_DOUBLE_EQ(date.fractionalDay, 0.);
}

TEST(DateTest, testMjdConversionsAt20240307)
{
    MjDate date;
    date.day = 0;
    date.fractionalDay = 0;
    AtomicTime tai = AtomicTime::afterEpoch(Duration::zero());

    // ************************
    // test at 2024-03-07
    // checked with astropy
    //>>> t = Time(60376.0, format="mjd", scale="tt")
    //>>> t.iso
    //'2024-03-07 00:00:00.000'
    //>>> t.tai.iso
    //'2024-03-06 23:59:27.816'
    //>>> t.utc.iso
    //'2024-03-06 23:58:50.816'
    //>>> t.tt.gps
    // 1393804748.816

    date.day = 60376;
    date.fractionalDay = 0.;

    // cppcheck-suppress redundantInitialization
    tai = MjDate::toTaiTime(date);

    // check the offset between TAI and TT
    // left part is TAI, right part is TT
    EXPECT_EQ(tai.timeSinceEpoch().microseconds() + MjDate::taiToTtAdd.microseconds(),
              ((2024 - 1958) * 365 + (2024 - 1958) / 4 - 1 /*no leap year 2000*/
               + 31 /*jan*/ + 29 /*feb*/ + 7 /*day of march*/)
                      * Duration::secondsPerDay * Duration::millisecondsPerSecond
                      * Duration::microsecondsPerMillisecond);

    // test the conversion MJD -> TAI -> UNIX -> utcDate
    auto unixTime = tai.convertTo<UnixTime>();
    auto utcDate = Date::fromUnixTime(unixTime);

    // check the conversions were successful
    EXPECT_EQ(utcDate.year, 2024);
    EXPECT_EQ(utcDate.month, 3);
    EXPECT_EQ(utcDate.day, 6);
    EXPECT_EQ(utcDate.hour, 23);
    EXPECT_EQ(utcDate.minute, 58);
    EXPECT_EQ(utcDate.second, 50);
    // the 0.184s are missing and cannot be represented in utcDate.

    // test conversion MJD -> TAI -> GPS
    auto gps = tai.convertTo<GpsTime>();
    EXPECT_EQ(gps.timeSinceEpoch().microseconds(), 1393804748816000);
}

TEST(DateTest, testMjdConversionsAt20240307T12_00_00)
{
    MjDate date;
    date.day = 0;
    date.fractionalDay = 0;
    AtomicTime tai = AtomicTime::afterEpoch(Duration::zero());

    // *************************
    // test with fractional days
    date.day = 60376;
    date.fractionalDay = 0.5;

    // test he conversion MJD -> TAI -> UNIX -> utcDate
    // cppcheck-suppress redundantInitialization
    tai = MjDate::toTaiTime(date);
    auto unixTime = tai.convertTo<UnixTime>();
    auto utcDate = Date::fromUnixTime(unixTime);

    EXPECT_EQ(utcDate.year, 2024);
    EXPECT_EQ(utcDate.month, 3);
    EXPECT_EQ(utcDate.day, 7);
    EXPECT_EQ(utcDate.hour, 11);
    EXPECT_EQ(utcDate.minute, 58);
    EXPECT_EQ(utcDate.second, 50);
    // the 0.184s are missing and cannot be represented in utcDate.
}

TEST(DateTest, testMjdConversionsAt20240307T00_00_01)
{
    MjDate date;
    date.day = 0;
    date.fractionalDay = 0;
    AtomicTime tai = AtomicTime::afterEpoch(Duration::zero());

    // *******************************
    // test one second after day begin
    date.day = 60376;
    date.fractionalDay = static_cast<double>(1.0) / Duration::secondsPerDay;

    // cppcheck-suppress redundantInitialization
    tai = MjDate::toTaiTime(date);
    auto unixTime = tai.convertTo<UnixTime>();
    auto utcDate = Date::fromUnixTime(unixTime);

    EXPECT_EQ(utcDate.year, 2024);
    EXPECT_EQ(utcDate.month, 3);
    EXPECT_EQ(utcDate.day, 6);
    EXPECT_EQ(utcDate.hour, 23);
    EXPECT_EQ(utcDate.minute, 58);
    EXPECT_EQ(utcDate.second, 51);  // the additional Second.
    // the 0.184s are missing and cannot be represented in utcDate.
}

TEST(DateTest, testMjdConversionsAt20240307T23_59_59)
{
    MjDate date;
    date.day = 0;
    date.fractionalDay = 0;
    AtomicTime tai = AtomicTime::afterEpoch(Duration::zero());

    // *********************************
    // test one second before end of day
    date.day = 60376;
    date.fractionalDay = static_cast<double>(Duration::secondsPerDay - 1) / Duration::secondsPerDay;

    // cppcheck-suppress redundantInitialization
    tai = MjDate::toTaiTime(date);
    auto unixTime = tai.convertTo<UnixTime>();
    auto utcDate = Date::fromUnixTime(unixTime);

    EXPECT_EQ(utcDate.year, 2024);
    EXPECT_EQ(utcDate.month, 3);
    EXPECT_EQ(utcDate.day, 7);
    EXPECT_EQ(utcDate.hour, 23);
    EXPECT_EQ(utcDate.minute, 58);
    EXPECT_EQ(utcDate.second, 49);  // one second less than the first test
    // the 0.184s are missing and cannot be represented in utcDate.
}

TEST(DateTest, testMjdDurationMinUnit)
{
    MjDate date;
    date.day = 0;
    date.fractionalDay = 0;
    AtomicTime tai = AtomicTime::afterEpoch(Duration::zero());

    // **********************
    // test one micro second.
    date.day = 60376;
    date.fractionalDay = static_cast<double>(1) / Duration::microsecondsPerDay;

    // cppcheck-suppress redundantInitialization
    tai = MjDate::toTaiTime(date);

    // check the offset between TAI and TT
    // left part in TAI, right part in TT
    EXPECT_EQ(tai.timeSinceEpoch().microseconds() + MjDate::taiToTtAdd.microseconds(),
              ((2024 - 1958) * 365 + (2024 - 1958) / 4 - 1 /*no leap year 2000*/
               + 31 /*jan*/ + 29 /*feb*/ + 7 /*day of march*/)
                              * Duration::secondsPerDay * Duration::millisecondsPerSecond
                              * Duration::microsecondsPerMillisecond
                      + 1 /* the micro second to test*/);
}

TEST(DateTest, testMjdAcceptsLargeFractionalDaysIn2024)
{
    MjDate date;
    date.day = 0;
    AtomicTime tai = AtomicTime::afterEpoch(Duration::zero());

    // ******************************************
    // Date may be represented as fractional only
    // a double at this date has an LSB that represents 0.628643 us. So it could be possible to pass
    // this test
    date.day = 0;

    // testing 1 minute takes 2 sec. so testing 1 day takes ~0.8 hours
    for (int64_t usOfDay = 0;
         usOfDay < Duration::microsecondsPerMillisecond * Duration::millisecondsPerSecond
                           * Duration::secondsPerMinute;
         ++usOfDay)
    {
        date.fractionalDay = static_cast<double>(60376)
                             + static_cast<double>(usOfDay) / Duration::microsecondsPerDay;

        tai = MjDate::toTaiTime(date);

        // left part in Tai from MJD, right part in TT to manually compare the correctness.
        // a double at this date has an LSB that represents 0.628643 us. So it should pass
        // the test, as the double oversamples the uint64.
        EXPECT_EQ(tai.timeSinceEpoch().microseconds() + MjDate::taiToTtAdd.microseconds(),
                  ((2024 - 1958) * 365 + (2024 - 1958) / 4
                   - 1 /*no leap year 2000*/ + 31 /*jan*/ + 29 /*feb*/ + 7 /*day of march*/)
                                  * Duration::secondsPerDay * Duration::millisecondsPerSecond
                                  * Duration::microsecondsPerMillisecond
                          + usOfDay /* the micro second to test*/);

        auto gps = tai.convertTo<GpsTime>();
        // a double at this date has an LSB that represents 0.628643 us. So it should pass
        // the test, as the double oversamples the uint64.
        EXPECT_EQ(gps.timeSinceEpoch().microseconds(),
                  1393804748816000 + usOfDay /* the micro second to test*/);
    }
}

TEST(DateTest, testConversionToMjd)
{
    MjDate date;
    date.day = 0;
    date.fractionalDay = 0;
    AtomicTime tai = AtomicTime::afterEpoch(Duration::zero());

    date.day = 60376;

    int64_t usOfDay = 1;
    date.fractionalDay = static_cast<double>(usOfDay) / Duration::microsecondsPerDay;
    // prepare MJD to TAI
    // cppcheck-suppress redundantInitialization
    tai = MjDate::toTaiTime(date);

    // *******************************
    // test back conversion TAI to MJD
    date = MjDate::fromTai(tai);

    EXPECT_EQ(date.day, 60376);
    EXPECT_DOUBLE_EQ(date.fractionalDay, static_cast<double>(usOfDay) / 86400000000.);

    int64_t sOfDay = 0;
    date.fractionalDay = static_cast<double>(sOfDay) / Duration::secondsPerDay;
    // prepare MJD to TAI
    tai = MjDate::toTaiTime(date);

    // ****************************************************
    // utcDate is not precise, so there is a different date
    auto unixTime = tai.convertTo<UnixTime>();
    auto utcDate = Date::fromUnixTime(unixTime);

    EXPECT_EQ(utcDate.year, 2024);
    EXPECT_EQ(utcDate.month, 3);
    EXPECT_EQ(utcDate.day, 6);
    EXPECT_EQ(utcDate.hour, 23);
    EXPECT_EQ(utcDate.minute, 58);
    // is truncated in conversion to seconds
    EXPECT_EQ(utcDate.second, 50);

    for (sOfDay = 0; sOfDay < 60; ++sOfDay)
    {
        // alter the date
        utcDate.minute = 59;
        utcDate.second = sOfDay;
        // ****************************************************
        // convert it back to MJD
        unixTime = Date::toUnixTime(utcDate);
        tai = unixTime.convertTo<AtomicTime>();
        date = MjDate::fromTai(tai);

        EXPECT_EQ(date.day, 60376);
        EXPECT_DOUBLE_EQ(
                date.fractionalDay,
                (10 * Duration::microsecondsPerMillisecond
                         * Duration::millisecondsPerSecond /*the 10 sec to the next minute*/
                 - 1000000                                 /* 1 sec that was truncated*/
                 //+ 60 /* the minute that was addad*/ * Duration::microsecondsPerMillisecond *
                 // Duration::millisecondsPerSecond
                 + sOfDay * Duration::microsecondsPerMillisecond * Duration::millisecondsPerSecond
                 + 184000 /*the ms that were lost*/)
                        / 86400000000.);
    }
}

// ---------------------------------------------------------------------------
TEST(DateTest, shouldConstructGpsDate)
{
    GpsDate date = {0U, 1U};

    ASSERT_THAT(date.weekNumber, ::testing::Eq(0U));
    ASSERT_THAT(date.timeOfWeek, ::testing::Eq(1U));
}
