/*
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/hal/datagram_transport.h>

#include <unittest/hal/datagram_transport_stub.h>
#include <unittest/harness.h>

using outpost::hal::DatagramTransport;

class DatagramTransportStubTest : public testing::Test
{
public:
    DatagramTransportStubTest() : mDatagramTransport()
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

    unittest::hal::DatagramTransportStub mDatagramTransport;
    std::array<uint8_t, 1000> mBuffer;
};

// ----------------------------------------------------------------------------
TEST_F(DatagramTransportStubTest, recieveFromShouldReturnErrorCodeWhenEmpty)
{
    DatagramTransport::Address senderAddressIfReceiveWasSuccessful;
    const auto maybePacket = mDatagramTransport.receiveFrom(outpost::asSlice(mBuffer),
                                                            senderAddressIfReceiveWasSuccessful,
                                                            outpost::time::Seconds(0));
    (void) senderAddressIfReceiveWasSuccessful;

    ASSERT_FALSE(maybePacket);
}

TEST_F(DatagramTransportStubTest, isAvailableReturnsErrorCodeWhenSet)
{
    mDatagramTransport.setErrorCode(DatagramTransport::ErrorCode::failure);
    ASSERT_FALSE(mDatagramTransport.isAvailable().has_value());

    mDatagramTransport.setErrorCode(DatagramTransport::ErrorCode::success);
    ASSERT_TRUE(mDatagramTransport.isAvailable().has_value());
}

TEST_F(DatagramTransportStubTest, getNumberOfBytesAvailableReturnsErrorCodeWhenSet)
{
    mDatagramTransport.setErrorCode(DatagramTransport::ErrorCode::failure);
    ASSERT_FALSE(mDatagramTransport.getNumberOfBytesAvailable().has_value());

    mDatagramTransport.setErrorCode(DatagramTransport::ErrorCode::success);
    ASSERT_TRUE(mDatagramTransport.getNumberOfBytesAvailable().has_value());
}

TEST_F(DatagramTransportStubTest, sendToReturnsErrorCodeWhenSet)
{
    DatagramTransport::Address address;

    mDatagramTransport.setErrorCode(DatagramTransport::ErrorCode::failure);
    auto maybe = mDatagramTransport.sendTo(
            outpost::asSlice(mBuffer), address, outpost::time::Seconds(0));
    ASSERT_FALSE(maybe.has_value());

    mDatagramTransport.setErrorCode(DatagramTransport::ErrorCode::success);
    maybe = mDatagramTransport.sendTo(
            outpost::asSlice(mBuffer), address, outpost::time::Seconds(0));
    ASSERT_TRUE(maybe.has_value());
}

TEST_F(DatagramTransportStubTest, receiveFromReturnsErrorCodeWhenSet)
{
    DatagramTransport::Address senderAddressIfReceiveWasSuccessful;
    mDatagramTransport.setErrorCode(DatagramTransport::ErrorCode::failure);
    auto maybe = mDatagramTransport.receiveFrom(outpost::asSlice(mBuffer),
                                                senderAddressIfReceiveWasSuccessful,
                                                outpost::time::Seconds(0));
    ASSERT_FALSE(maybe.has_value());
    (void) senderAddressIfReceiveWasSuccessful;
}
