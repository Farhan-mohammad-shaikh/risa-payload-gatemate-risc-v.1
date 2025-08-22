/*
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/comm/rmap/rmap_initiator.h>

#include <gmock/gmock.h>

#include <unittest/hal/spacewire_stub.h>
#include <unittest/harness.h>
#include <unittest/swb/testing_software_bus.h>
#include <unittest/time/testing_clock.h>

using namespace outpost;
using namespace outpost::comm;

class RmapInitiatorTest : public testing::Test
{
public:
    static constexpr uint8_t someLogicalAddress = 0x2;

    RmapInitiatorTest() :
        mSpaceWire{100},
        mHandler{mSpaceWire, 100, support::parameter::HeartbeatSource::default0, mClock},
        mRmapTarget{
                "bob",               // name
                1,                   // id
                someLogicalAddress,  // targetLogicalAddress
                3                    // key
        },
        dut{mHandler,
            &mTargetNodes,
            100,   // thread priority
            4096,  // thread stack size
            support::parameter::HeartbeatSource::default0,
            someLogicalAddress}
    {
        mTargetNodes.addTargetNode(&mRmapTarget);
    }

    virtual void
    SetUp() override
    {
        mSpaceWire.open();
        mSpaceWire.up(time::Duration::zero());
        // mRmapTarget.setTargetSpaceWireAddress(asSlice(targetSpwAddress));
        // mRmapTarget.setReplyAddress(asSlice(replyAddress));
    }

    unittest::time::TestingClock mClock;
    unittest::hal::SpaceWireStub mSpaceWire;
    hal::SpaceWireMultiProtocolHandler<2> mHandler;
    RmapTargetNode mRmapTarget;
    RmapTargetsList mTargetNodes;

    RmapInitiator dut;
};

TEST_F(RmapInitiatorTest, canBeInitialized)
{
    // Nothing registered
    ASSERT_EQ(mHandler.getNumberOfChannels(), 0U);
    // EXPECT_TRUE(dut.isStopped());
    // EXPECT_FALSE(dut.isStarted());
}

TEST_F(RmapInitiatorTest, init)
{
    // Nothing registered
    ASSERT_EQ(mHandler.getNumberOfChannels(), 0U);

    dut.init();

    EXPECT_EQ(mHandler.getNumberOfChannels(), 1U);
    // EXPECT_FALSE(dut.isStopped());
    // EXPECT_TRUE(dut.isStarted());
}

TEST_F(RmapInitiatorTest, invalidWrite)
{
    RmapResult result =
            dut.write(mRmapTarget, RMapOptions{}, 0, 0, outpost::Slice<const uint8_t>::empty());
    EXPECT_FALSE(result);
    EXPECT_EQ(result.getResult(), RmapResult::Code::invalidParameters);
}

// TODO: Others
