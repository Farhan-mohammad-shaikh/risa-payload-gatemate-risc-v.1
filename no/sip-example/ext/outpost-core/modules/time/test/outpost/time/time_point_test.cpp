/*
 * Copyright (c) 2015-2017, 2019, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <harness/time.h>

#include <outpost/time/time_point.h>

#include <unittest/harness.h>

using namespace ::testing;
using namespace outpost::time;

TEST(TimePointTest, shouldCreateFromDuration)
{
    SpacecraftElapsedTime time = SpacecraftElapsedTime::afterEpoch(Milliseconds(1456));

    EXPECT_EQ(Milliseconds(1456), time.timeSinceEpoch());
}

//  TimePoint - TimePoint --> Duration
TEST(TimePointTest, subtractionShouldGiveADuration)
{
    SpacecraftElapsedTime time1 = SpacecraftElapsedTime::afterEpoch(Milliseconds(100));
    SpacecraftElapsedTime time2 = SpacecraftElapsedTime::afterEpoch(Milliseconds(200));

    Duration d = time2 - time1;

    EXPECT_EQ(Milliseconds(100), d);
}

// TimePoint + Duration  --> TimePoint
TEST(TimePointTest, shouldAddDurationToTimePoint)
{
    SpacecraftElapsedTime time1 = SpacecraftElapsedTime::afterEpoch(Milliseconds(200));
    Duration d = Microseconds(12345);

    SpacecraftElapsedTime time2 = time1 + d;

    EXPECT_EQ(Microseconds(212345), time2.timeSinceEpoch());
}

// TimePoint - Duration  --> TimePoint
TEST(TimePointTest, shouldSubtractDurationFromTimePoint)
{
    SpacecraftElapsedTime time1 = SpacecraftElapsedTime::afterEpoch(Milliseconds(200));
    Duration d = Microseconds(12345);

    SpacecraftElapsedTime time2 = time1 - d;

    EXPECT_EQ(Microseconds(187655), time2.timeSinceEpoch());
}

TEST(TimePointTest, shouldConvertEpoch)
{
    TimeEpochConverter<SpacecraftElapsedTimeEpoch, GpsEpoch>::setOffset(Microseconds(200));

    SpacecraftElapsedTime timeScet = SpacecraftElapsedTime::startOfEpoch();
    GpsTime timeGps = timeScet.convertTo<GpsTime>();

    EXPECT_EQ(200, timeGps.timeSinceEpoch().microseconds());

    // Reset offset
    TimeEpochConverter<SpacecraftElapsedTimeEpoch, GpsEpoch>::setOffset(Duration::zero());
}

TEST(TimePointTest, shouldBeComparable)
{
    SpacecraftElapsedTime time1 = SpacecraftElapsedTime::afterEpoch(Milliseconds(100));
    SpacecraftElapsedTime time2 = SpacecraftElapsedTime::afterEpoch(Milliseconds(200));

    EXPECT_TRUE(time1 == time1);
    EXPECT_FALSE(time1 == time2);
    EXPECT_FALSE(time1 != time1);
    EXPECT_TRUE(time1 != time2);

    EXPECT_TRUE(time1 < time2);
    EXPECT_TRUE(time1 <= time2);
    EXPECT_FALSE(time1 > time2);
    EXPECT_FALSE(time1 >= time2);

    EXPECT_FALSE(time2 < time1);
    EXPECT_FALSE(time2 <= time1);
    EXPECT_TRUE(time2 > time1);
    EXPECT_TRUE(time2 >= time1);

    EXPECT_FALSE(time1 < time1);
    EXPECT_TRUE(time1 <= time1);
    EXPECT_FALSE(time1 > time1);
    EXPECT_TRUE(time1 >= time1);
}

TEST(TimePointTest, comparisonShouldNotUnderflowWithMin)
{
    SpacecraftElapsedTime time1 = SpacecraftElapsedTime::afterEpoch(-Duration::maximum());
    SpacecraftElapsedTime time2 = SpacecraftElapsedTime::afterEpoch(Milliseconds(200));

    EXPECT_TRUE(time1 == time1);
    EXPECT_FALSE(time1 == time2);
    EXPECT_FALSE(time1 != time1);
    EXPECT_TRUE(time1 != time2);

    EXPECT_TRUE(time1 < time2);
    EXPECT_TRUE(time1 <= time2);
    EXPECT_FALSE(time1 > time2);
    EXPECT_FALSE(time1 >= time2);

    EXPECT_FALSE(time2 < time1);
    EXPECT_FALSE(time2 <= time1);
    EXPECT_TRUE(time2 > time1);
    EXPECT_TRUE(time2 >= time1);

    EXPECT_FALSE(time1 < time1);
    EXPECT_TRUE(time1 <= time1);
    EXPECT_FALSE(time1 > time1);
    EXPECT_TRUE(time1 >= time1);
}

TEST(TimePointTest, comparisonShouldNotOverflowWithMax)
{
    SpacecraftElapsedTime time1 = SpacecraftElapsedTime::afterEpoch(Milliseconds(100));
    SpacecraftElapsedTime time2 = SpacecraftElapsedTime::afterEpoch(Duration::maximum());

    EXPECT_TRUE(time1 == time1);
    EXPECT_FALSE(time1 == time2);
    EXPECT_FALSE(time1 != time1);
    EXPECT_TRUE(time1 != time2);

    EXPECT_TRUE(time1 < time2);
    EXPECT_TRUE(time1 <= time2);
    EXPECT_FALSE(time1 > time2);
    EXPECT_FALSE(time1 >= time2);

    EXPECT_FALSE(time2 < time1);
    EXPECT_FALSE(time2 <= time1);
    EXPECT_TRUE(time2 > time1);
    EXPECT_TRUE(time2 >= time1);

    EXPECT_FALSE(time2 < time2);
    EXPECT_TRUE(time2 <= time2);
    EXPECT_FALSE(time2 > time2);
    EXPECT_TRUE(time2 >= time2);
}

// ---------------------------------------------------------------------------
TEST(TimeEpochTest, shouldPrintSpacecraftElapsedTime)
{
    SpacecraftElapsedTime time = SpacecraftElapsedTime::startOfEpoch() + 100_ms;
    const auto asString = ::testing::PrintToString(time);
    ASSERT_THAT(asString, StrEq("100ms after system start"));
}

TEST(TimeEpochTest, shouldPrintSpacecraftElapsedTimeNegative)
{
    SpacecraftElapsedTime time = SpacecraftElapsedTime::startOfEpoch() - 100_ms;
    const auto asString = ::testing::PrintToString(time);
    ASSERT_THAT(asString, StrEq("100ms before system start"));
}

TEST(TimeEpochTest, shouldPrintUnixTime)
{
    UnixTime time = UnixTime::startOfEpoch() + 3_h + 2_min + 1100_ms;
    const auto asString = ::testing::PrintToString(time);
    ASSERT_THAT(asString, StrEq("1970-01-01 03:02:01.100000"));
}

TEST(TimeEpochTest, shouldPrintUnixTimeNegative)
{
    UnixTime time = UnixTime::startOfEpoch() - 100_ms;
    const auto asString = ::testing::PrintToString(time);
    ASSERT_THAT(asString, StrEq("100ms before UNIX epoch"));
}

TEST(TimeEpochTest, shouldPrintGpsAsDuration)
{
    GpsTime time = GpsTime::startOfEpoch() + 100_ms;
    const auto asString = ::testing::PrintToString(time);
    ASSERT_THAT(asString, StrEq("100ms"));
}
