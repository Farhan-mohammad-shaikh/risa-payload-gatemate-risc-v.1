/*
 * Copyright (c) 2015-2017, Fabian Greif
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <unittest/harness.h>
#include <unittest/smpc/testing_subscription.h>
#include <unittest/smpc/topic_logger.h>

using namespace outpost::smpc;

struct Data
{
    int mValue;
};

static Topic<Data> globalTopic;

class TopicLoggerTest : public testing::Test
{
public:
    TopicLoggerTest() : mLogger(globalTopic)
    {
    }

    virtual void
    SetUp() override
    {
        unittest::smpc::TestingSubscription::connectSubscriptionsToTopics();
    }

    virtual void
    TearDown() override
    {
        EXPECT_TRUE(mLogger.isEmpty());

        unittest::smpc::TestingSubscription::releaseAllSubscriptions();
    }

    unittest::smpc::TopicLogger<Data> mLogger;
};

TEST_F(TopicLoggerTest, shouldReceiveSingleItem)
{
    Data data = {1};
    globalTopic.publish(data);

    ASSERT_FALSE(mLogger.isEmpty());

    auto& result = mLogger.getNext();

    EXPECT_EQ(data.mValue, result.mValue);
    mLogger.dropNext();
}

TEST_F(TopicLoggerTest, shouldReceiveMultiple)
{
    Data data1 = {1};
    Data data2 = {2};
    Data data3 = {3};
    Data data4 = {4};

    globalTopic.publish(data1);
    globalTopic.publish(data2);
    globalTopic.publish(data3);
    globalTopic.publish(data4);

    ASSERT_FALSE(mLogger.isEmpty());
    auto& result1 = mLogger.getNext();
    EXPECT_EQ(data1.mValue, result1.mValue);
    mLogger.dropNext();

    ASSERT_FALSE(mLogger.isEmpty());
    auto& result2 = mLogger.getNext();
    EXPECT_EQ(data2.mValue, result2.mValue);
    mLogger.dropNext();

    ASSERT_FALSE(mLogger.isEmpty());
    auto& result3 = mLogger.getNext();
    EXPECT_EQ(data3.mValue, result3.mValue);
    mLogger.dropNext();

    ASSERT_FALSE(mLogger.isEmpty());
    auto& result4 = mLogger.getNext();
    EXPECT_EQ(data4.mValue, result4.mValue);
    mLogger.dropNext();
}
