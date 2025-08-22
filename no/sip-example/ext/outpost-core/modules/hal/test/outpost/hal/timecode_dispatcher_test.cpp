/*
 * Copyright (c) 2019, Jan Malburg
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

#include <outpost/hal/timecode_dispatcher.h>
#include <outpost/rtos/queue.h>

#include <unittest/harness.h>

// Helper function
static bool
operator==(const outpost::hal::TimeCode& a, const outpost::hal::TimeCode& b)
{
    return (a.mControl == b.mControl) && (a.mValue == b.mValue);
}

TEST(TimeCodeDispatcherTest, dispatchWithoutQueue)
{
    outpost::hal::TimeCode tc{0, 0};
    outpost::hal::TimeCodeDispatcher<2> tcd;
    tcd.dispatchTimeCode(tc);
    // does this test anything?
}

TEST(TimeCodeDispatcherTest, dispatchSingleQueue)
{
    outpost::hal::TimeCode tc;

    tc.mControl = 1;
    tc.mValue = 14;

    outpost::hal::TimeCodeDispatcher<2> tcd;
    outpost::rtos::Queue<outpost::hal::TimeCode> queue(2);

    EXPECT_TRUE(tcd.addListener(&queue));
    tcd.dispatchTimeCode(tc);
    outpost::hal::TimeCode tmp;
    // set tmp to different values
    tmp.mControl = 0;
    tmp.mValue = 0;
    EXPECT_TRUE(queue.receive(tmp, outpost::time::Seconds(1)));

    EXPECT_TRUE(tc == tmp);
}

TEST(TimeCodeDispatcherTest, dispatchDoubleQueue)
{
    outpost::hal::TimeCode tc;

    tc.mControl = 1;
    tc.mValue = 14;

    outpost::hal::TimeCodeDispatcher<2> tcd;
    outpost::rtos::Queue<outpost::hal::TimeCode> queue1(2);
    outpost::rtos::Queue<outpost::hal::TimeCode> queue2(2);

    EXPECT_TRUE(tcd.addListener(&queue1));
    EXPECT_TRUE(tcd.addListener(&queue2));
    tcd.dispatchTimeCode(tc);
    outpost::hal::TimeCode tmp;
    // set tmp to different values
    tmp.mControl = 0;
    tmp.mValue = 0;
    EXPECT_TRUE(queue1.receive(tmp, outpost::time::Seconds(1)));
    EXPECT_TRUE(tc == tmp);

    // set tmp to different values
    tmp.mControl = 0;
    tmp.mValue = 0;

    EXPECT_TRUE(queue2.receive(tmp, outpost::time::Seconds(1)));
    EXPECT_TRUE(tc == tmp);
}

TEST(TimeCodeDispatcherTest, tooManyQueues)
{
    outpost::hal::TimeCodeDispatcher<2> tcd;
    outpost::rtos::Queue<outpost::hal::TimeCode> queue(2);

    EXPECT_TRUE(tcd.addListener(&queue));
    EXPECT_TRUE(tcd.addListener(&queue));
    EXPECT_FALSE(tcd.addListener(&queue));
}
