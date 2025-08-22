/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
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

struct Data
{
    uint32_t foo;
    uint16_t bar;
};

static bool
operator==(Data const& lhs, Data const& rhs)
{
    return ((lhs.foo == rhs.foo) && (lhs.bar == rhs.bar));
}

class Component : public outpost::smpc::Subscriber
{
public:
    explicit Component(outpost::smpc::Topic<const Data>& topic) :
        mReceived(false), mData({0, 0}), subscription(topic, this, &Component::onReceiveData)
    {
    }

    void
    onReceiveData(const Data* data)
    {
        mReceived = true;
        mData = *data;
    }

    bool mReceived;
    Data mData;

private:
    outpost::smpc::Subscription subscription;
};

TEST(CommunicationTest, receiveSingle)
{
    outpost::smpc::Topic<const Data> topic;
    Component component(topic);

    outpost::smpc::Subscription::connectSubscriptionsToTopics();

    Data data = {0x12345678, 0x9876};

    topic.publish(data);

    ASSERT_TRUE(component.mReceived);
    EXPECT_EQ(data, component.mData);

    unittest::smpc::TestingSubscription::releaseAllSubscriptions();
}
