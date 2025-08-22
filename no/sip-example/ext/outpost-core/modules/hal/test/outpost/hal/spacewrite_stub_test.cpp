/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2021, Thomas Firchau
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <unittest/hal/spacewire_stub.h>
#include <unittest/harness.h>

using outpost::hal::SpaceWire;

class SpaceWireStubTest : public testing::Test
{
public:
    SpaceWireStubTest() : mSpaceWire(100)
    {
    }

    virtual void
    SetUp() override
    {
        mSpaceWire.open();
        mSpaceWire.up(outpost::time::Duration::zero());
    }

    virtual void
    TearDown() override
    {
    }

    unittest::hal::SpaceWireStub mSpaceWire;
};

// ----------------------------------------------------------------------------
TEST(SpaceWireStubConnectionTest, shouldEnableLink)
{
    unittest::hal::SpaceWireStub spaceWire(100);
    EXPECT_TRUE(spaceWire.open());
}

TEST(SpaceWireStubConnectionTest, shouldBringLinkUp)
{
    unittest::hal::SpaceWireStub spaceWire(100);

    spaceWire.open();
    spaceWire.up(outpost::time::Duration::zero());

    EXPECT_TRUE(spaceWire.isUp());
}

TEST(SpaceWireStubConnectionTest, shouldBringLinkDown)
{
    unittest::hal::SpaceWireStub spaceWire(100);

    spaceWire.open();

    spaceWire.up(outpost::time::Duration::zero());
    spaceWire.down(outpost::time::Duration::zero());

    EXPECT_FALSE(spaceWire.isUp());
}

TEST(SpaceWireStubConnectionTest, shouldBringLinkDownOnClose)
{
    unittest::hal::SpaceWireStub spaceWire(100);

    spaceWire.open();
    spaceWire.up(outpost::time::Duration::zero());

    spaceWire.close();

    EXPECT_FALSE(spaceWire.isUp());
}

TEST(SpaceWireStubConnectionTest, shouldNotBringLinkUpOnClosedChannel)
{
    unittest::hal::SpaceWireStub spaceWire(100);

    spaceWire.up(outpost::time::Duration::zero());

    EXPECT_FALSE(spaceWire.isUp());
}

// ----------------------------------------------------------------------------
TEST_F(SpaceWireStubTest, shouldProvideTransmitBuffer)
{
    auto result = mSpaceWire.requestBuffer(outpost::time::Duration::zero());
    ASSERT_TRUE(result);
    SpaceWire::TransmitBuffer* buffer = *result;
    ASSERT_NE(0U, buffer->getData().getNumberOfElements());
    EXPECT_FALSE(mSpaceWire.noUsedTransmitBuffers());
}

TEST_F(SpaceWireStubTest, shouldReleaseTransmitBuffer)
{
    auto result = mSpaceWire.requestBuffer(outpost::time::Duration::zero());
    ASSERT_TRUE(result);
    SpaceWire::TransmitBuffer* buffer = *result;
    ASSERT_EQ(SpaceWire::OperationResult::success,
              mSpaceWire.send(buffer, outpost::time::Duration::zero()));

    EXPECT_TRUE(mSpaceWire.noUsedTransmitBuffers());
}

TEST_F(SpaceWireStubTest, shouldTransmitData)
{
    std::vector<uint8_t> expectedData = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xAB, 0xCD};

    auto result = mSpaceWire.requestBuffer(outpost::time::Duration::zero());
    ASSERT_TRUE(result);
    SpaceWire::TransmitBuffer* buffer = *result;

    ASSERT_TRUE(buffer->getData().copyFrom(&expectedData.front(), expectedData.size()));
    buffer->setLength(expectedData.size());
    buffer->setEndMarker(SpaceWire::eop);

    mSpaceWire.send(buffer, outpost::time::Duration::zero());

    ASSERT_EQ(1U, mSpaceWire.mSentPackets.size());

    auto& packet = mSpaceWire.mSentPackets.front();
    EXPECT_EQ(expectedData, packet.data);
    EXPECT_EQ(SpaceWire::eop, packet.end);
}

TEST_F(SpaceWireStubTest, shouldReceiveTimeout)
{
    SpaceWire::ReceiveBuffer buffer;
    auto result = mSpaceWire.receive(outpost::time::Duration::zero());
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error(), SpaceWire::OperationResult::timeout);
}

TEST_F(SpaceWireStubTest, shouldReceiveData)
{
    std::vector<uint8_t> expectedData = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xAB, 0xCD};

    mSpaceWire.mPacketsToReceive.emplace_back(
            unittest::hal::SpaceWireStub::Packet{expectedData, SpaceWire::eep});

    auto result = mSpaceWire.receive(outpost::time::Duration::zero());
    ASSERT_TRUE(result);
    SpaceWire::ReceiveBuffer buffer = *result;

    EXPECT_EQ(expectedData.size(), buffer.getLength());
    EXPECT_EQ(SpaceWire::eep, buffer.getEndMarker());
    ASSERT_THAT(expectedData,
                testing::ElementsAreArray(buffer.getData().begin(), buffer.getLength()));

    mSpaceWire.releaseBuffer(buffer);

    EXPECT_TRUE(mSpaceWire.mPacketsToReceive.empty());
    EXPECT_TRUE(mSpaceWire.noUsedReceiveBuffers());
}
