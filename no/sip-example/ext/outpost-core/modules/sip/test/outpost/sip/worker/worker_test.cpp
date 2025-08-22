/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2024, Felix Passenberg
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/rtos.h>
#include <outpost/sip/packet_transport/packet_transport_wrapper.h>
#include <outpost/sip/worker/worker.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder_hdlc.h>
#include <outpost/transport/frame_coder/frame_encoder_hdlc.h>
#include <outpost/transport/frame_transport/frame_transport_serial.h>

#include <unittest/hal/serial_stub.h>
#include <unittest/harness.h>

class TestWorker : public ::testing::Test
{
public:
    TestWorker() :
        mWorkerId(0x55),
        bufferedFrameDecoderHdlc(outpost::asSlice(receiveBuffer)),
        frameTransportSerial(clock,
                             serial,
                             frameEncoderHdlc,
                             outpost::asSlice(transmitBuffer),
                             bufferedFrameDecoderHdlc),
        packetTransportWrapper(frameTransportSerial),
        worker(mWorkerId, packetTransportWrapper)
    {
    }

    uint8_t mWorkerId;

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

    outpost::sip::Worker worker;
};

TEST_F(TestWorker, sendResponse)
{
    EXPECT_TRUE(worker.sendResponse(2, 3));

    EXPECT_EQ(serial.mDataToTransmit[0], 0x7E);
    EXPECT_EQ(serial.mDataToTransmit[1], 0);
    EXPECT_EQ(serial.mDataToTransmit[2], 5);
    EXPECT_EQ(serial.mDataToTransmit[3], mWorkerId);
    EXPECT_EQ(serial.mDataToTransmit[4], 2);
    EXPECT_EQ(serial.mDataToTransmit[5], 3);
    // CRC
    // EXPECT_EQ(serial.mDataToTransmit[6], 0);
    // EXPECT_EQ(serial.mDataToTransmit[7], 0);
    EXPECT_EQ(serial.mDataToTransmit[8], 0x7E);
}
