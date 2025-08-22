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

#include <outpost/parameter/sip.h>
#include <outpost/sip/packet/packet_reader.h>
#include <outpost/sip/packet_transport/packet_transport_wrapper.h>

#include <unittest/harness.h>

using namespace ::testing;

class TestPacketTransportWrapper : public ::testing::Test
{
public:
    TestPacketTransportWrapper() :
        packetTransportWrapper(frameTransportTxMock, frameTransportRxMock),
        packetTransportTxWrapper(frameTransportTxMock),
        packetTransportRxWrapper(frameTransportRxMock)
    {
    }

    harness::transport::FrameTransportTxMock frameTransportTxMock;
    harness::transport::FrameTransportRxMock frameTransportRxMock;

    outpost::sip::PacketTransportWrapper packetTransportWrapper;
    outpost::sip::PacketTransportTxWrapper packetTransportTxWrapper;
    outpost::sip::PacketTransportRxWrapper packetTransportRxWrapper;
};

TEST_F(TestPacketTransportWrapper, transmitError)
{
    EXPECT_CALL(frameTransportTxMock, transmit(_))
            .WillOnce(Return(
                    outpost::unexpected(outpost::transport::OperationResult::streamStopped)));

    std::array<uint8_t, 4> payload = {0, 1, 2, 3};
    const outpost::sip::PacketReader packet(outpost::asSlice(payload));

    auto maybeBytes = packetTransportWrapper.transmit(packet);
    ASSERT_FALSE(maybeBytes);
    ASSERT_EQ(maybeBytes.error(), outpost::sip::OperationResult::transportError);
}
