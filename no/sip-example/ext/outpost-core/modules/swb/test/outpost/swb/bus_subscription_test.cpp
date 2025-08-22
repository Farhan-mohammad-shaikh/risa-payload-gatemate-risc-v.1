/*
 * Copyright (c) 2020-2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/swb/bus_subscription.h>

#include <unittest/harness.h>

class BusSubscriptionTest : public ::testing::Test
{
public:
    BusSubscriptionTest()
    {
    }

    virtual void
    SetUp() override
    {
    }

    virtual void
    TearDown() override
    {
    }
};

using MessageId = uint16_t;

TEST_F(BusSubscriptionTest, constructorTest)
{
    MessageId id = 0x0100;

    {
        outpost::swb::BusSubscription<MessageId> sub1(id);
        EXPECT_EQ(sub1.getNumberOfMatchedMessages(), 0U);
    }

    outpost::swb::BusSubscription<MessageId> sub2(id, 0xFF00);
    EXPECT_EQ(sub2.getNumberOfMatchedMessages(), 0U);
}

TEST_F(BusSubscriptionTest, matchTest)
{
    MessageId id = 0x1234;
    outpost::swb::BusSubscription<MessageId> sub1(id);

    EXPECT_FALSE(sub1.matches(0U));
    EXPECT_FALSE(sub1.matches(0x1235));
    EXPECT_FALSE(sub1.matches(0x1233));
    EXPECT_FALSE(sub1.matches(0xFFFF));
    EXPECT_FALSE(sub1.matches(0x0234));
    EXPECT_FALSE(sub1.matches(0x1200));

    EXPECT_EQ(sub1.getNumberOfMatchedMessages(), 0U);

    EXPECT_TRUE(sub1.matches(0x1234));

    EXPECT_EQ(sub1.getNumberOfMatchedMessages(), 1U);
}

TEST_F(BusSubscriptionTest, matchTestWithMask)
{
    MessageId id = 0x1234;
    outpost::swb::BusSubscription<MessageId> sub1(id, 0xFF00);

    EXPECT_FALSE(sub1.matches(0U));
    EXPECT_FALSE(sub1.matches(0xFFFF));
    EXPECT_FALSE(sub1.matches(0x0233));

    EXPECT_TRUE(sub1.matches(0x1200));
    EXPECT_TRUE(sub1.matches(0x1235));
    EXPECT_TRUE(sub1.matches(0x1233));
    EXPECT_TRUE(sub1.matches(0x12FF));
    EXPECT_TRUE(sub1.matches(0x1234));

    EXPECT_EQ(sub1.getNumberOfMatchedMessages(), 5U);

    outpost::swb::BusSubscription<MessageId> sub2(id, 0x00FF);

    EXPECT_FALSE(sub2.matches(0U));
    EXPECT_FALSE(sub2.matches(0xFFFF));
    EXPECT_FALSE(sub2.matches(0x1200));
    EXPECT_FALSE(sub2.matches(0x1235));
    EXPECT_FALSE(sub2.matches(0x1233));
    EXPECT_FALSE(sub2.matches(0x12FF));

    EXPECT_TRUE(sub2.matches(0x0234));
    EXPECT_TRUE(sub2.matches(0x1034));
    EXPECT_TRUE(sub2.matches(0x0034));
    EXPECT_TRUE(sub2.matches(0x1234));

    EXPECT_EQ(sub2.getNumberOfMatchedMessages(), 4U);
}
