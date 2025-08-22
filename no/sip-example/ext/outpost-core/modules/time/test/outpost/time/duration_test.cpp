/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2015, Annika Ofenloch
 * Copyright (c) 2021, Andre Nahrwold
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

#include <harness/time.h>

#include <unittest/harness.h>

using namespace ::testing;

// ---------------------------------------------------------------------------
TEST(DurationTest, shouldAddTwoDurations)
{
    outpost::time::Duration d1 = outpost::time::Milliseconds(100);
    outpost::time::Duration d2 = outpost::time::Microseconds(23456);

    outpost::time::Duration d3 = d1 + d2;

    EXPECT_EQ(outpost::time::Microseconds(123456), d3);

    EXPECT_EQ(outpost::time::Milliseconds(100), d1);
    EXPECT_EQ(outpost::time::Microseconds(23456), d2);
}

TEST(DurationTest, derivedTypes)
{
    ASSERT_EQ(outpost::time::Seconds(5), outpost::time::Milliseconds(5000));
    ASSERT_EQ(outpost::time::Microseconds(1000), outpost::time::Milliseconds(1));

    outpost::time::Duration duration = outpost::time::Seconds(5);

    ASSERT_EQ(5, duration.seconds());
    ASSERT_EQ(5000, duration.milliseconds());
    ASSERT_EQ(5000000, duration.microseconds());
}

TEST(DurationTest, shouldConvertToMinutes)
{
    // rand() to force run-time eval of functions
    const unsigned hours = rand() % 10 + 1;
    const unsigned minutes = hours * 60;
    const unsigned seconds = minutes * 60;

    outpost::time::Duration duration = outpost::time::Seconds(seconds);
    outpost::time::Duration theSameThingInMinutes = outpost::time::Minutes(minutes);
    outpost::time::Duration theSameThingInHours = outpost::time::Hours(hours);

    EXPECT_EQ(seconds, duration.seconds());
    EXPECT_EQ(minutes, duration.minutes());
    EXPECT_EQ(hours, duration.hours());

    EXPECT_EQ(duration, theSameThingInMinutes);
    EXPECT_EQ(duration, theSameThingInHours);
}

TEST(DurationTest, shouldConvertToHours)
{
    outpost::time::Duration duration = outpost::time::Seconds(3600);

    ASSERT_EQ(1, duration.hours());
}

TEST(DurationTest, convertBigNumbersMicrosecondsToDuration)
{
    outpost::time::Duration out = outpost::time::Microseconds(3147483643);

    EXPECT_EQ(3147483643, out.microseconds());
}

TEST(DurationTest, shouldHaveNoRemainder)
{
    outpost::time::Duration d1 = outpost::time::Seconds(16);
    outpost::time::Duration d2 = outpost::time::Seconds(4);

    EXPECT_EQ(outpost::time::Duration::zero(), d1 % d2);
}

TEST(DurationTest, shouldHaveRemainder)
{
    outpost::time::Duration d1 = outpost::time::Seconds(16);
    outpost::time::Duration d2 = outpost::time::Seconds(5);

    EXPECT_EQ(outpost::time::Seconds(1), d1 % d2);
}

TEST(DurationTest, myriadIsAReallyLongTimeInTheFuture)
{
    const auto never = outpost::time::Duration::myriad();
    const auto otherNever = outpost::time::Duration::maximum();  // is expected to be deprecated!
    const auto farFarAway = outpost::time::Hours(10000000);      // like, a thousand years or so?

    EXPECT_GT(never, farFarAway);
    EXPECT_GT(otherNever, farFarAway);

    EXPECT_GT(otherNever, never);        // just based on internal numbers, but I'll be dead anyway
    testing::internal::CaptureStderr();  // address sanitizer may correctly detect this overflow
    EXPECT_LT(otherNever + outpost::time::Seconds(1),
              never + outpost::time::Seconds(1));  // *LESS*, because of overflow!
    auto maybeAsanError = testing::internal::GetCapturedStderr();
    if (maybeAsanError.size() > 0)
    {
        EXPECT_THAT(maybeAsanError, testing::HasSubstr("runtime error: signed integer overflow"));
    }
}

// ---------------------------------------------------------------------------
TEST(DurationTest, shouldCreateDurationFrom_us)
{
    using namespace outpost::time::literals;

    outpost::time::Duration duration = 101_us;
    ASSERT_THAT(duration, Eq(outpost::time::Microseconds(101)));
}

TEST(DurationTest, shouldCreateNegativeDurationFrom_us)
{
    using namespace outpost::time::literals;

    outpost::time::Duration duration = -101_us;
    ASSERT_THAT(duration, Eq(-outpost::time::Microseconds(101)));
}

TEST(DurationTest, shouldCreateDurationFrom_ms)
{
    using namespace outpost::time::literals;

    outpost::time::Duration duration = 101_ms;
    ASSERT_THAT(duration, Eq(outpost::time::Milliseconds(101)));
}

TEST(DurationTest, shouldCreateDurationFrom_s)
{
    using namespace outpost::time::literals;

    outpost::time::Duration duration = 101_s;
    ASSERT_THAT(duration, Eq(outpost::time::Seconds(101)));
}

TEST(DurationTest, shouldCreateDurationFrom_min)
{
    using namespace outpost::time::literals;

    outpost::time::Duration duration = 101_min;
    ASSERT_THAT(duration, Eq(outpost::time::Minutes(101)));
}

TEST(DurationTest, shouldCreateDurationFrom_h)
{
    using namespace outpost::time::literals;

    outpost::time::Duration duration = 101_h;
    ASSERT_THAT(duration, Eq(outpost::time::Hours(101)));
}

// ---------------------------------------------------------------------------
TEST(DurationTest, shouldPrintZero)
{
    outpost::time::Duration zero = outpost::time::Duration::zero();
    std::string asString = ::testing::PrintToString(zero);
    ASSERT_THAT(asString, StrEq("zero"));
}

TEST(DurationTest, shouldPrintInfinity)
{
    outpost::time::Duration infinity = outpost::time::Duration::maximum();
    std::string asString = ::testing::PrintToString(infinity);
    ASSERT_THAT(asString, StrEq("maximum/infinity"));
}

TEST(DurationTest, shouldPrintMyriad)
{
    outpost::time::Duration myriad = outpost::time::Duration::myriad();
    std::string asString = ::testing::PrintToString(myriad);
    ASSERT_THAT(asString, StrEq("1 myriad"));
}

TEST(DurationTest, shouldPrintMyriadMinusShortDuration)
{
    outpost::time::Duration myriad =
            outpost::time::Duration::myriad() - outpost::time::Milliseconds(100);
    std::string asString = ::testing::PrintToString(myriad);
    ASSERT_THAT(asString, StrEq("1 myriad - 100ms"));
}

TEST(DurationTest, shouldPrintMyriadPlusShortDuration)
{
    outpost::time::Duration myriad =
            outpost::time::Duration::myriad() + outpost::time::Milliseconds(100);
    std::string asString = ::testing::PrintToString(myriad);
    ASSERT_THAT(asString, StrEq("1 myriad + 100ms"));
}

TEST(DurationTest, shouldPrintNegativeMyriadMinusShortDuration)
{
    outpost::time::Duration myriad =
            -outpost::time::Duration::myriad() - outpost::time::Milliseconds(100);
    std::string asString = ::testing::PrintToString(myriad);
    ASSERT_THAT(asString, StrEq("-1 myriad - 100ms"));
}

TEST(DurationTest, shouldPrintNegativeMyriadPlusShortDuration)
{
    outpost::time::Duration myriad =
            -outpost::time::Duration::myriad() + outpost::time::Milliseconds(100);
    std::string asString = ::testing::PrintToString(myriad);
    ASSERT_THAT(asString, StrEq("-1 myriad + 100ms"));
}

TEST(DurationTest, shouldPrintMultipleMyriadsMinusShortDuration)
{
    outpost::time::Duration myriad =
            3 * outpost::time::Duration::myriad() - outpost::time::Milliseconds(100);
    std::string asString = ::testing::PrintToString(myriad);
    ASSERT_THAT(asString, StrEq("3 myriad - 100ms"));
}

TEST(DurationTest, shouldPrintMultipleMyriadsPlusShortDuration)
{
    outpost::time::Duration myriad =
            3 * outpost::time::Duration::myriad() + outpost::time::Milliseconds(100);
    std::string asString = ::testing::PrintToString(myriad);
    ASSERT_THAT(asString, StrEq("3 myriad + 100ms"));
}

TEST(DurationTest, shouldPrintMultipleNegativeMyriadsMinusShortDuration)
{
    outpost::time::Duration myriad =
            3 * -outpost::time::Duration::myriad() - outpost::time::Milliseconds(100);
    std::string asString = ::testing::PrintToString(myriad);
    ASSERT_THAT(asString, StrEq("-3 myriad - 100ms"));
}

TEST(DurationTest, shouldPrintMultipleNegativeMyriadsPlusShortDuration)
{
    outpost::time::Duration myriad =
            3 * -outpost::time::Duration::myriad() + outpost::time::Milliseconds(100);
    std::string asString = ::testing::PrintToString(myriad);
    ASSERT_THAT(asString, StrEq("-3 myriad + 100ms"));
}

TEST(DurationTest, shouldPrintMicroseconds)
{
    outpost::time::Duration durationWithUs = outpost::time::Microseconds(100000001U);
    std::string asString = ::testing::PrintToString(durationWithUs);
    ASSERT_THAT(asString, StrEq("100000001us"));
}

TEST(DurationTest, shouldNotPrintMicrosecondsButMilliseconds)
{
    outpost::time::Duration durationWithoutUs = outpost::time::Microseconds(100001000U);
    std::string asString = ::testing::PrintToString(durationWithoutUs);
    ASSERT_THAT(asString, StrEq("100001ms"));
}

TEST(DurationTest, shouldPrintMilliseconds)
{
    outpost::time::Duration durationWithMs = outpost::time::Milliseconds(100000001U);
    std::string asString = ::testing::PrintToString(durationWithMs);
    ASSERT_THAT(asString, StrEq("100000001ms"));
}

TEST(DurationTest, shouldNotPrintMillisecondsButSeconds)
{
    outpost::time::Duration durationWithoutMs = outpost::time::Milliseconds(100001000U);
    std::string asString = ::testing::PrintToString(durationWithoutMs);
    ASSERT_THAT(asString, StrEq("100001s"));
}

TEST(DurationTest, shouldPrintSeconds)
{
    outpost::time::Duration durationWithSeconds = outpost::time::Seconds(100000001U);
    std::string asString = ::testing::PrintToString(durationWithSeconds);
    ASSERT_THAT(asString, StrEq("100000001s"));
}

TEST(DurationTest, shouldNotPrintSecondsButMinutes)
{
    outpost::time::Duration durationWithoutSeconds = outpost::time::Seconds(360);
    std::string asString = ::testing::PrintToString(durationWithoutSeconds);
    ASSERT_THAT(asString, StrEq("6min"));
}

TEST(DurationTest, shouldPrintMinutes)
{
    outpost::time::Duration durationWithMinutes = outpost::time::Minutes(100000001U);
    std::string asString = ::testing::PrintToString(durationWithMinutes);
    ASSERT_THAT(asString, StrEq("100000001min"));
}

TEST(DurationTest, shouldNotPrintMinutesButHours)
{
    outpost::time::Duration durationWithoutMinutes = outpost::time::Minutes(360);
    std::string asString = ::testing::PrintToString(durationWithoutMinutes);
    ASSERT_THAT(asString, StrEq("6h"));
}
