/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2023, Felix Passenberg
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

namespace subscription_test
{
struct Data
{
    uint32_t foo;
    uint16_t bar;
};

class Component : public outpost::smpc::Subscriber
{
public:
    Component() : received()
    {
        reset();
    }

    void
    onReceiveData0(const Data* d)
    {
        received[0] = true;
        foo[0] = d->foo;
        bar[0] = d->bar;
    }

    void
    onReceiveData1(const Data* d)
    {
        received[1] = true;
        foo[1] = d->foo;
        bar[1] = d->bar;
    }

    void
    onReceiveData2(const Data* d)
    {
        received[2] = true;
        foo[2] = d->foo;
        bar[2] = d->bar;
    }

    void
    onReceiveData3(const Data* d)
    {
        received[3] = true;
        foo[3] = d->foo;
        bar[3] = d->bar;
    }

    void
    reset()
    {
        for (int i = 0; i < 4; ++i)
        {
            received[i] = false;
            foo[i] = 0;
            bar[i] = 0;
        }
    }

    bool received[4];
    uint32_t foo[4];
    uint16_t bar[4];
};

class SubscriptionTest : public ::testing::Test
{
public:
    SubscriptionTest() : data({0x12345678, 0x9876})
    {
    }

    virtual void
    SetUp()
    {
        component.reset();
    }

    virtual void
    TearDown()
    {
        unittest::smpc::TestingSubscription::releaseAllSubscriptions();
    }

    Component component;
    Data data;
    outpost::smpc::Topic<const Data> topic;
};

TEST_F(SubscriptionTest, receiveNone)
{
    topic.publish(data);
    for (int i = 0; i < 4; ++i)
    {
        EXPECT_FALSE(component.received[i]);
    }

    unittest::smpc::TestingSubscription::connectSubscriptionsToTopics();

    topic.publish(data);
    for (int i = 0; i < 4; ++i)
    {
        EXPECT_FALSE(component.received[i]);
    }
}

TEST_F(SubscriptionTest, receiveTwo)
{
    outpost::smpc::Subscription* subscription0 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData0);
    outpost::smpc::Subscription* subscription1 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData1);

    unittest::smpc::TestingSubscription::connectSubscriptionsToTopics();

    topic.publish(data);
    EXPECT_TRUE(component.received[0]);
    EXPECT_EQ(component.foo[0], 0x12345678u);
    EXPECT_EQ(component.bar[0], 0x9876u);

    EXPECT_TRUE(component.received[1]);
    EXPECT_EQ(component.foo[1], 0x12345678u);
    EXPECT_EQ(component.bar[1], 0x9876u);

    EXPECT_FALSE(component.received[2]);
    EXPECT_EQ(component.foo[2], 0u);
    EXPECT_EQ(component.bar[2], 0u);

    EXPECT_FALSE(component.received[3]);
    EXPECT_EQ(component.foo[3], 0u);
    EXPECT_EQ(component.bar[3], 0u);

    delete subscription0;
    delete subscription1;
}

TEST_F(SubscriptionTest, receiveFour)
{
    outpost::smpc::Subscription* subscription0 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData0);
    outpost::smpc::Subscription* subscription1 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData1);
    outpost::smpc::Subscription* subscription2 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData2);
    outpost::smpc::Subscription* subscription3 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData3);

    unittest::smpc::TestingSubscription::connectSubscriptionsToTopics();

    topic.publish(data);
    EXPECT_TRUE(component.received[0]);
    EXPECT_TRUE(component.received[1]);
    EXPECT_TRUE(component.received[2]);
    EXPECT_TRUE(component.received[3]);

    delete subscription0;
    delete subscription1;
    delete subscription2;
    delete subscription3;
}

TEST_F(SubscriptionTest, receiveFourWithDelete)
{
    outpost::smpc::Subscription* subscription0 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData0);
    outpost::smpc::Subscription* subscription1 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData1);
    outpost::smpc::Subscription* subscription2 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData2);
    outpost::smpc::Subscription* subscription3 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData3);

    unittest::smpc::TestingSubscription::connectSubscriptionsToTopics();

    delete subscription1;

    topic.publish(data);
    EXPECT_TRUE(component.received[0]);
    EXPECT_FALSE(component.received[1]);
    EXPECT_TRUE(component.received[2]);
    EXPECT_TRUE(component.received[3]);

    delete subscription0;
    delete subscription2;
    delete subscription3;
}

TEST_F(SubscriptionTest, receiveFourWithDelete2)
{
    outpost::smpc::Subscription* subscription0 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData0);
    outpost::smpc::Subscription* subscription1 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData1);
    outpost::smpc::Subscription* subscription2 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData2);
    outpost::smpc::Subscription* subscription3 =
            new outpost::smpc::Subscription(topic, &component, &Component::onReceiveData3);

    unittest::smpc::TestingSubscription::connectSubscriptionsToTopics();

    delete subscription0;
    delete subscription3;

    topic.publish(data);
    EXPECT_FALSE(component.received[0]);
    EXPECT_TRUE(component.received[1]);
    EXPECT_TRUE(component.received[2]);
    EXPECT_FALSE(component.received[3]);

    delete subscription1;
    delete subscription2;
}
}  // namespace subscription_test
