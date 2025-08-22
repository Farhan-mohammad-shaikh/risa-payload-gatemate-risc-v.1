/*
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
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

#include <outpost/swb/default_message_filter.h>
#include <outpost/swb/software_bus.h>

#include <unittest/harness.h>
#include <unittest/swb/testing_software_bus.h>

using namespace outpost::swb;

class FilteredSoftwareBusTest : public ::testing::Test
{
public:
    FilteredSoftwareBusTest()
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

    outpost::container::SharedBufferPool<1024, 20> mPool;
    outpost::container::ReferenceQueue<Message<uint16_t>, 10U> mQueue;
};

using MessageId = uint16_t;

TEST_F(FilteredSoftwareBusTest, constructor)
{
    SoftwareBusFiltered<MessageId, RangeFilter<MessageId>> bus(
            mPool, mQueue, 123U, outpost::support::parameter::HeartbeatSource::default0);

    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 0U);
    EXPECT_EQ(bus.getNumberOfChannels(), 0U);
    EXPECT_EQ(bus.getNumberOfFailedCopyOperations(), 0U);
    EXPECT_EQ(bus.getNumberOfFailedSendOperations(), 0U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 0U);
}

TEST_F(FilteredSoftwareBusTest, unconfiguredFilter)
{
    SoftwareBusFiltered<MessageId, RangeFilter<MessageId>> bus(
            mPool, mQueue, 123U, outpost::support::parameter::HeartbeatSource::default0);

    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 0U);
    EXPECT_EQ(bus.getNumberOfChannels(), 0U);
    EXPECT_EQ(bus.getNumberOfFailedCopyOperations(), 0U);
    EXPECT_EQ(bus.getNumberOfFailedSendOperations(), 0U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 0U);

    outpost::container::ConstSharedBufferPointer p;
    Message<MessageId> m{1, p};

    EXPECT_EQ(OperationResult::invalidMessage, bus.sendMessage(m));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 0U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 1U);

    EXPECT_EQ(OperationResult::invalidMessage, bus.sendMessage(1, p));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 0U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 2U);

    outpost::Slice<uint8_t> emptySlice = outpost::Slice<uint8_t>::empty();
    EXPECT_EQ(OperationResult::invalidMessage, bus.sendMessage(1, emptySlice));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 0U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 3U);
}

TEST_F(FilteredSoftwareBusTest, configureFilter)
{
    SoftwareBusFiltered<MessageId, RangeFilter<MessageId>> bus(
            mPool, mQueue, 123U, outpost::support::parameter::HeartbeatSource::default0);

    MessageId min = 100U;
    MessageId max = 200U;

    bus.getFilter().setRange(min, max);

    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 0U);
    EXPECT_EQ(bus.getNumberOfChannels(), 0U);
    EXPECT_EQ(bus.getNumberOfFailedCopyOperations(), 0U);
    EXPECT_EQ(bus.getNumberOfFailedSendOperations(), 0U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 0U);

    // Send valid max
    outpost::container::ConstSharedBufferPointer p;
    Message<MessageId> m1{max, p};

    EXPECT_EQ(OperationResult::success, bus.sendMessage(m1));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 1U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 0U);

    EXPECT_EQ(OperationResult::success, bus.sendMessage(max, p));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 2U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 0U);

    outpost::Slice<uint8_t> emptySlice = outpost::Slice<uint8_t>::empty();
    EXPECT_EQ(OperationResult::success, bus.sendMessage(max, emptySlice));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 3U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 0U);

    // Send valid min
    Message<MessageId> m2{min, p};

    EXPECT_EQ(OperationResult::success, bus.sendMessage(m2));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 4U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 0U);

    EXPECT_EQ(OperationResult::success, bus.sendMessage(min, p));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 5U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 0U);

    EXPECT_EQ(OperationResult::success, bus.sendMessage(min, emptySlice));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 6U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 0U);

    // Send invalid max+1
    MessageId invalid = max + 1;
    Message<MessageId> m3{invalid, p};

    EXPECT_EQ(OperationResult::invalidMessage, bus.sendMessage(m3));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 6U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 1U);

    EXPECT_EQ(OperationResult::invalidMessage, bus.sendMessage(invalid, p));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 6U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 2U);

    EXPECT_EQ(OperationResult::invalidMessage, bus.sendMessage(invalid, emptySlice));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 6U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 3U);

    // Send invalid min-1
    invalid = min - 1;
    Message<MessageId> m4{invalid, p};

    EXPECT_EQ(OperationResult::invalidMessage, bus.sendMessage(m4));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 6U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 4U);

    EXPECT_EQ(OperationResult::invalidMessage, bus.sendMessage(invalid, p));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 6U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 5U);

    EXPECT_EQ(OperationResult::invalidMessage, bus.sendMessage(invalid, emptySlice));
    EXPECT_EQ(bus.getNumberOfAcceptedMessages(), 6U);
    EXPECT_EQ(bus.getNumberOfDeclinedMessages(), 6U);

    unittest::swb::TestingSoftwareBus testing(bus);

    for (size_t i = 0; i < 6; i++)
    {
        testing.singleMessage();

        EXPECT_EQ(bus.getNumberOfHandledMessages(), i + 1U);
        EXPECT_EQ(bus.getNumberOfForwardedMessages(), 0U);
    }

    testing.singleMessage();

    EXPECT_EQ(bus.getNumberOfHandledMessages(), 6U);
    EXPECT_EQ(bus.getNumberOfForwardedMessages(), 0U);
}
