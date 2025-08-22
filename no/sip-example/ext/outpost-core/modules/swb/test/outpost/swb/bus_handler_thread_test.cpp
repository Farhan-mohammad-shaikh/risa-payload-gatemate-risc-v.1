/*
 * Copyright (c) 2020-2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/container/reference_queue.h>
#include <outpost/container/shared_object_pool.h>
#include <outpost/swb/bus_channel.h>
#include <outpost/swb/bus_subscription.h>
#include <outpost/swb/software_bus.h>

#include <unittest/harness.h>
#include <unittest/swb/testing_software_bus.h>
#include <unittest/time/testing_clock.h>

#include <cstdlib>
#include <future>
#include <utility>

static unittest::time::TestingClock Clock;

class BusHandlerThreadTest : public ::testing::Test
{
public:
    BusHandlerThreadTest()
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

using namespace outpost::swb;

// ----------------------------------------------------------------------------
TEST_F(BusHandlerThreadTest, handlerTest)
{
    outpost::container::SharedBufferPool<1024, 10> pool;
    outpost::container::ReferenceQueue<outpost::swb::Message<uint16_t>, 10> queue;
    outpost::swb::SoftwareBusFiltered<uint16_t, RangeFilter<uint16_t>> bus(
            pool, queue, 1U, outpost::support::parameter::HeartbeatSource::default0);

    unittest::swb::TestingSoftwareBus testingBus(bus);
    bus.getFilter().setRange(1, 100);

    outpost::swb::BufferedBusChannelWithMemory<10, uint16_t, SubscriptionFilter<uint16_t>> channel;
    outpost::swb::BusSubscription<uint16_t> subscription(1U);
    channel.getFilter().registerSubscription(subscription);
    EXPECT_TRUE(bus.registerChannel(channel) == outpost::swb::OperationResult::success);

    testingBus.singleMessage();
    outpost::swb::Message<uint16_t> m;
    EXPECT_FALSE(channel.receiveMessage(m) == outpost::swb::OperationResult::success);

    outpost::container::SharedBufferPointer p;
    pool.allocate(p);
    for (size_t i = 0; i < 16U; i++)
    {
        p[i] = i;
    }

    outpost::swb::Message<uint16_t> sendMessage = {2U, p};
    EXPECT_EQ(outpost::swb::OperationResult::success, bus.sendMessage(sendMessage));
    testingBus.singleMessage();

    EXPECT_FALSE(channel.receiveMessage(m) == outpost::swb::OperationResult::success);

    sendMessage = {1U, p};
    EXPECT_EQ(outpost::swb::OperationResult::success, bus.sendMessage(sendMessage));
    testingBus.singleMessage();

    EXPECT_TRUE(channel.receiveMessage(m) == outpost::swb::OperationResult::success);
    EXPECT_EQ(m.id, 1U);
    for (size_t i = 0; i < 16U; i++)
    {
        EXPECT_EQ(m.buffer[i], i);
    }

    sendMessage = {0U, p};
    EXPECT_EQ(outpost::swb::OperationResult::invalidMessage, bus.sendMessage(sendMessage));
    testingBus.singleMessage();

    EXPECT_TRUE(channel.receiveMessage(m) == outpost::swb::OperationResult::noMessageAvailable);

    sendMessage = {101U, p};
    EXPECT_EQ(outpost::swb::OperationResult::invalidMessage, bus.sendMessage(sendMessage));
    testingBus.singleMessage();

    EXPECT_TRUE(channel.receiveMessage(m) == outpost::swb::OperationResult::noMessageAvailable);
}
