/*
 * Copyright (c) 2018, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/smpc/subscription.h>
#include <outpost/smpc/topic.h>

#include <unittest/harness.h>
#include <unittest/smpc/testing_subscription.h>

#include <stdint.h>
#include <string.h>

class ComponentBase : public outpost::smpc::Subscriber
{
public:
    virtual ~ComponentBase() = default;

    virtual void
    onReceiveData(const bool*)
    {
        mReceivedBase = true;
    }

    bool mReceivedBase = false;
};

class ComponentChild : public ComponentBase
{
public:
    virtual void
    onReceiveData(const bool*) override
    {
        mReceivedChild = true;
    }

    bool mReceivedChild = false;
};

class SubscriptionVirtualClassTest : public ::testing::Test
{
public:
    virtual void
    SetUp()
    {
        componentBase.mReceivedBase = false;
        componentChild.mReceivedBase = false;
        componentChild.mReceivedChild = false;
    }

    virtual void
    TearDown()
    {
        unittest::smpc::TestingSubscription::releaseAllSubscriptions();
    }

    ComponentBase componentBase;
    ComponentChild componentChild;

    outpost::smpc::Topic<const bool> topic;
};

TEST_F(SubscriptionVirtualClassTest, baseClassShouldReceiveData)
{
    outpost::smpc::Subscription subscription(topic, &componentBase, &ComponentBase::onReceiveData);
    unittest::smpc::TestingSubscription::connectSubscriptionsToTopics();

    topic.publish(false);
    EXPECT_TRUE(componentBase.mReceivedBase);

    unittest::smpc::TestingSubscription::releaseAllSubscriptions();
}

TEST_F(SubscriptionVirtualClassTest, childClassShouldReceiveData)
{
    outpost::smpc::Subscription subscription(
            topic, &componentChild, &ComponentChild::onReceiveData);
    unittest::smpc::TestingSubscription::connectSubscriptionsToTopics();

    topic.publish(false);

    EXPECT_FALSE(componentChild.mReceivedBase);
    EXPECT_TRUE(componentChild.mReceivedChild);

    unittest::smpc::TestingSubscription::releaseAllSubscriptions();
}

TEST_F(SubscriptionVirtualClassTest, childClassShouldReceiveDataThroughBase)
{
    outpost::smpc::Subscription subscription(topic, &componentChild, &ComponentBase::onReceiveData);
    unittest::smpc::TestingSubscription::connectSubscriptionsToTopics();

    topic.publish(false);

    EXPECT_FALSE(componentChild.mReceivedBase);
    EXPECT_TRUE(componentChild.mReceivedChild);

    unittest::smpc::TestingSubscription::releaseAllSubscriptions();
}
