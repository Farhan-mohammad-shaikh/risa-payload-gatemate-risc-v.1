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

#include <harness/transport/frame_transport/frame_transport_mock.h>

#include <unittest/hal/serial_stub.h>
#include <unittest/harness.h>

using namespace ::testing;

class TestFrameTransportMock : public ::testing::Test
{
public:
};

TEST_F(TestFrameTransportMock, ReceiveIsMocked)
{
    harness::transport::FrameTransportMock frameTransportMock;

    std::array<uint8_t, 4> incomingBuffer = {1, 2, 3, 4};
    auto incomingData = outpost::asSlice(incomingBuffer);

    EXPECT_CALL(frameTransportMock, receive(_, _))
            .WillOnce(harness::transport::FrameTransportMock::ReceiveFrom(incomingData));

    std::array<uint8_t, 128> receiveBuffer;
    auto maybeReceivedData =
            frameTransportMock.receive(outpost::asSlice(receiveBuffer), outpost::time::Seconds(1));

    EXPECT_TRUE(maybeReceivedData);
    EXPECT_TRUE(SliceMatch(incomingData, *maybeReceivedData));
}

TEST_F(TestFrameTransportMock, TransmitIsMocked)
{
    harness::transport::FrameTransportMock frameTransportMock;

    std::array<uint8_t, 128> transmitBuffer;

    EXPECT_CALL(frameTransportMock, transmit(_))
            .WillOnce(harness::transport::FrameTransportMock::TransmitTo(
                    outpost::asSlice(transmitBuffer)));

    std::array<uint8_t, 4> outgoingBuffer = {1, 2, 3, 4};
    auto outgoingData = outpost::asSlice(outgoingBuffer);
    auto maybeBytesTransmitted = frameTransportMock.transmit(outgoingData);

    EXPECT_TRUE(maybeBytesTransmitted);

    auto transmittedData = outpost::asSlice(transmitBuffer).first(*maybeBytesTransmitted);

    EXPECT_TRUE(SliceMatch(outgoingData, transmittedData));
}
