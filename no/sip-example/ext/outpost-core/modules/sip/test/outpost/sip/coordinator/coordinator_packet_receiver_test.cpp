/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022-2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/parameter/sip.h>
#include <outpost/rtos.h>
#include <outpost/sip/sip.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder_hdlc.h>
#include <outpost/transport/frame_coder/frame_encoder_hdlc.h>
#include <outpost/transport/frame_transport/frame_transport_serial.h>

#include <unittest/hal/serial_stub.h>
#include <unittest/harness.h>

class TestCoordinatorPacketReceiver : public ::testing::Test
{
public:
    TestCoordinatorPacketReceiver() :
        bufferedFrameDecoderHdlc(outpost::asSlice(receiveBuffer)),
        frameTransportSerial(clock,
                             serial,
                             frameEncoderHdlc,
                             outpost::asSlice(transmitBuffer),
                             bufferedFrameDecoderHdlc),
        packetTransportWrapper(frameTransportSerial),
        coordinator(packetTransportWrapper),
        coordinatorPacketReceiver(packetTransportWrapper,
                                  coordinator,
                                  outpost::support::parameter::HeartbeatSource::default0,
                                  140)
    {
    }

    outpost::rtos::SystemClock clock;

    unittest::hal::SerialStub serial;

    outpost::transport::FrameEncoderHdlc frameEncoderHdlc;
    outpost::transport::BufferedFrameDecoderHdlc bufferedFrameDecoderHdlc;

    constexpr static size_t worseCaseFrameSize = outpost::sip::parameter::maxPacketLength * 2
                                                 + outpost::coding::HdlcStuffing::boundary_overhead;
    std::array<uint8_t, worseCaseFrameSize> transmitBuffer;
    std::array<uint8_t, worseCaseFrameSize> receiveBuffer;
    outpost::transport::FrameTransportSerial frameTransportSerial;

    outpost::sip::PacketTransportWrapper packetTransportWrapper;

    outpost::sip::Coordinator coordinator;

    outpost::sip::CoordinatorPacketReceiver coordinatorPacketReceiver;

    uint8_t queueError = 0;
    uint8_t readError = 1;
    uint8_t receiveError = 2;
    uint8_t success = 3;
    uint8_t unknown = 4;

    uint8_t
    testReceivePacket()
    {
        switch (coordinatorPacketReceiver.receivePacket(outpost::time::Seconds(1)))
        {
            case outpost::sip::CoordinatorPacketReceiver::receiveResult::queueError:
                return queueError;
                break;
            case outpost::sip::CoordinatorPacketReceiver::receiveResult::readError:
                return readError;
                break;
            case outpost::sip::CoordinatorPacketReceiver::receiveResult::receiveError:
                return receiveError;
                break;
            case outpost::sip::CoordinatorPacketReceiver::receiveResult::success:
                return success;
                break;
        }
        return unknown;
    }
};

TEST_F(TestCoordinatorPacketReceiver, receiveError)
{
    EXPECT_EQ(testReceivePacket(), receiveError);
}

TEST_F(TestCoordinatorPacketReceiver, ErrorInvalidPacket)
{
    serial.mDataToReceive.clear();
    serial.mDataToReceive.push_back(0x7E);
    serial.mDataToReceive.push_back(0x55);
    serial.mDataToReceive.push_back(0x7E);

    EXPECT_EQ(testReceivePacket(), receiveError);
}

TEST_F(TestCoordinatorPacketReceiver, success)
{
    uint8_t bufferWriter[outpost::sip::parameter::maxPacketLength];
    outpost::Slice<uint8_t> bufferPacketWriter(bufferWriter);
    outpost::sip::PacketWriter packetWriter(bufferPacketWriter);

    packetWriter.setWorkerId(1);
    packetWriter.setCounter(2);
    packetWriter.setType(3);

    packetWriter.update();
    auto const maybeSlice = packetWriter.getSliceIfFinalized();
    ASSERT_TRUE(maybeSlice);
    outpost::Slice<const uint8_t> bufferToWrite = *maybeSlice;

    serial.mDataToReceive.clear();
    serial.mDataToReceive.push_back(0x7E);
    // length
    serial.mDataToReceive.push_back(bufferToWrite[0]);
    serial.mDataToReceive.push_back(bufferToWrite[1]);
    // id
    serial.mDataToReceive.push_back(bufferToWrite[2]);
    // counter
    serial.mDataToReceive.push_back(bufferToWrite[3]);
    // type
    serial.mDataToReceive.push_back(bufferToWrite[4]);
    // CRC
    serial.mDataToReceive.push_back(bufferToWrite[5]);
    serial.mDataToReceive.push_back(bufferToWrite[6]);
    serial.mDataToReceive.push_back(0x7E);

    EXPECT_EQ(testReceivePacket(), success);
}
