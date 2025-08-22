/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2022, Passenberg, Felix Constantin
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Tepe, Alexander
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <harness/time.h>

#include <outpost/time/timeout.h>

#include <unittest/harness.h>
#include <unittest/time/testing_clock.h>

using namespace outpost::time;

class TimeoutTest : public testing::Test
{
public:
    unittest::time::TestingClock mClock;
};

TEST_F(TimeoutTest, shouldBeStoppedAfterDefaultConstruction)
{
    Timeout timeout(mClock);

    EXPECT_TRUE(timeout.isStopped());
    EXPECT_EQ(Timeout::State::stopped, timeout.getState());
}

TEST_F(TimeoutTest, shouldBeArmedAfterConstructionWithTime)
{
    Timeout timeout(mClock, 100_ms);

    EXPECT_TRUE(timeout.isArmed());
    EXPECT_EQ(Timeout::State::armed, timeout.getState());
}

TEST_F(TimeoutTest, shouldExpiredAfterGivenTime)
{
    Timeout timeout(mClock, 100_ms);

    mClock.setTime(SpacecraftElapsedTime::afterEpoch(100_ms));

    EXPECT_TRUE(timeout.isExpired());
    EXPECT_EQ(Timeout::State::expired, timeout.getState());
}

TEST_F(TimeoutTest, shouldRestartFromStopped)
{
    Timeout timeout(mClock);

    timeout.restart(100_ms);

    mClock.setTime(SpacecraftElapsedTime::afterEpoch(100_ms));

    EXPECT_TRUE(timeout.isExpired());
    EXPECT_EQ(Timeout::State::expired, timeout.getState());
}

TEST_F(TimeoutTest, shouldRestartFromExpired)
{
    Timeout timeout(mClock, 100_ms);
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(100_ms));
    EXPECT_TRUE(timeout.isExpired());

    timeout.restart(100_ms);
    EXPECT_FALSE(timeout.isExpired());
    EXPECT_EQ(Timeout::State::armed, timeout.getState());

    mClock.setTime(SpacecraftElapsedTime::afterEpoch(200_ms));
    EXPECT_TRUE(timeout.isExpired());
}

TEST_F(TimeoutTest, ExtendDurationBeforeExpiry)
{
    Timeout timeout(mClock, 100_ms);
    EXPECT_TRUE(timeout.isArmed());
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(50_ms));
    timeout.changeDuration(200_ms);
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(150_ms));
    EXPECT_FALSE(timeout.isExpired());

    mClock.setTime(SpacecraftElapsedTime::afterEpoch(200_ms));
    EXPECT_TRUE(timeout.isExpired());
}

TEST_F(TimeoutTest, ExtendDurationAfterExpiry)
{
    Timeout timeout(mClock, 100_ms);
    EXPECT_TRUE(timeout.isArmed());
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(150_ms));
    timeout.changeDuration(200_ms);
    EXPECT_TRUE(timeout.isExpired());
}

TEST_F(TimeoutTest, ReduceDurationToExpire)
{
    Timeout timeout(mClock, 100_ms);
    EXPECT_TRUE(timeout.isArmed());
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(50_ms));
    timeout.changeDuration(10_ms);
    EXPECT_TRUE(timeout.isExpired());
}

TEST_F(TimeoutTest, ReduceDurationToNotExpire)
{
    Timeout timeout(mClock, 100_ms);
    EXPECT_TRUE(timeout.isArmed());
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(50_ms));
    timeout.changeDuration(80_ms);
    EXPECT_FALSE(timeout.isExpired());

    mClock.setTime(SpacecraftElapsedTime::afterEpoch(80_ms));
    EXPECT_TRUE(timeout.isExpired());
}

TEST_F(TimeoutTest, DelayedStartAndRestart)
{
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(500_ms));
    Timeout timeout(mClock, 100_ms);
    EXPECT_TRUE(timeout.isArmed());
    timeout.changeDuration(80_ms);
    EXPECT_FALSE(timeout.isExpired());

    mClock.setTime(SpacecraftElapsedTime::afterEpoch(580_ms));
    EXPECT_TRUE(timeout.isExpired());
}

TEST_F(TimeoutTest, ConstTimeout)
{
    const Timeout timeout(mClock, 100_ms);
    // checking availability of const interfaces
    EXPECT_TRUE(timeout.isArmed());
    EXPECT_FALSE(timeout.isStopped());
    EXPECT_FALSE(timeout.isExpired());
    EXPECT_EQ(Timeout::State::armed, timeout.getState());

    // checking const timeout can expire
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(110_ms));
    EXPECT_TRUE(timeout.isExpired());
    EXPECT_EQ(Timeout::State::expired, timeout.getState());
}

TEST_F(TimeoutTest, shouldGiveRemainingTime)
{
    Timeout timeout(mClock);
    timeout.changeDuration(100_ms);
    ASSERT_EQ(timeout.getRemainingTime(), 100_ms);
}

TEST_F(TimeoutTest, shouldReturnRemainingTime)
{
    Timeout timeout(mClock);
    timeout.restart(200_ms);
    outpost::time::Duration remaining;
    ASSERT_FALSE(timeout.isExpired(remaining));
    ASSERT_EQ(remaining, 200_ms);
}

TEST_F(TimeoutTest, getRemainingTimeShouldNotReturnNegativeValue)
{
    const Timeout timeout(mClock, 100_ms);
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(150_ms));
    outpost::time::Duration duration = timeout.getRemainingTime();
    ASSERT_EQ(duration, outpost::time::Duration::zero());
}

TEST_F(TimeoutTest, expiredTimeoutShouldReturnZero)
{
    const Timeout timeout(mClock, 100_ms);
    mClock.setTime(SpacecraftElapsedTime::afterEpoch(100_ms));
    outpost::time::Duration duration;
    ASSERT_TRUE(timeout.isExpired(duration));
    ASSERT_EQ(outpost::time::Duration::zero(), duration);
}

TEST_F(TimeoutTest, instantiatingNegativeTimeoutShouldDie)
{
    EXPECT_OUTPOST_ASSERTION_DEATH(Timeout timeout(mClock, -100_ms), "Negative timeout");
}

TEST_F(TimeoutTest, restaringWithNegativeDurationShouldDie)
{
    Timeout timeout(mClock, 100_ms);
    EXPECT_OUTPOST_ASSERTION_DEATH(timeout.restart(-100_ms), "Negative timeout");
}

TEST_F(TimeoutTest, changeDurationToNegativeValueShouldDie)
{
    Timeout timeout(mClock);
    EXPECT_OUTPOST_ASSERTION_DEATH(timeout.changeDuration(-100_ms), "Negative timeout");
}

TEST_F(TimeoutTest, isExpiredOnStoppedTimeoutShouldDie)
{
    Timeout timeout(mClock, 100_ms);
    timeout.stop();
    outpost::time::Duration duration;
    EXPECT_OUTPOST_ASSERTION_DEATH(timeout.isExpired(duration), "Timeout not running");
}

TEST_F(TimeoutTest, isExpiredWithoutParamDiesWhenTimeoutStopped)
{
    const Timeout timeout(mClock);
    EXPECT_OUTPOST_ASSERTION_DEATH(timeout.isExpired(), "Timeout not running");
}
