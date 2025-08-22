/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022-2024, Jan-Gerd Mess
 * Copyright (c) 2023-2024, Felix Passenberg
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
#include <outpost/sip/coordinator/coordinator.h>
#include <outpost/sip/packet_transport/packet_transport_wrapper.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder_hdlc.h>
#include <outpost/transport/frame_coder/frame_encoder_hdlc.h>
#include <outpost/transport/frame_transport/frame_transport_serial.h>

#include <unittest/hal/serial_stub.h>
#include <unittest/harness.h>

class TestCoordinator : public ::testing::Test
{
public:
    TestCoordinator() :
        bufferedFrameDecoderHdlc(outpost::asSlice(receiveBuffer)),
        frameTransportSerial(clock,
                             serial,
                             frameEncoderHdlc,
                             outpost::asSlice(transmitBuffer),
                             bufferedFrameDecoderHdlc),
        packetTransportWrapper(frameTransportSerial),
        coordinator(packetTransportWrapper)
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
};

TEST_F(TestCoordinator, responseError)
{
    EXPECT_EQ(coordinator.sendRequest(1, 2, 3, 4), outpost::sip::OperationResult::responseError);
}

TEST_F(TestCoordinator, success)
{
    const uint8_t workerId = 1;
    const uint8_t counter = 2;
    const uint8_t type = 3;
    const uint8_t expectedType = 4;

    outpost::sip::Coordinator::ResponseData data;
    data.length = 5;
    data.workerId = workerId;
    data.counter = counter;
    data.type = expectedType;

    EXPECT_TRUE(coordinator.sendResponseQueue(data));

    EXPECT_EQ(coordinator.sendRequest(workerId, counter, type, expectedType),
              outpost::sip::OperationResult::success);

    // test the correct serialization
    EXPECT_EQ(serial.mDataToTransmit[0], 0x7e);
    EXPECT_EQ(serial.mDataToTransmit[1], 0x00);
    EXPECT_EQ(serial.mDataToTransmit[2], 0x05);
    EXPECT_EQ(serial.mDataToTransmit[3], workerId);
    EXPECT_EQ(serial.mDataToTransmit[4], counter);
    EXPECT_EQ(serial.mDataToTransmit[5], type);
    EXPECT_EQ(serial.mDataToTransmit[6], 0xcc);  // crc
    EXPECT_EQ(serial.mDataToTransmit[7], 0x78);  // crc
    EXPECT_EQ(serial.mDataToTransmit[8], 0x7e);
}

TEST_F(TestCoordinator, workerIdError)
{
    const uint8_t workerId = 1;
    const uint8_t counter = 2;
    const uint8_t type = 3;
    const uint8_t expectedType = 4;

    outpost::sip::Coordinator::ResponseData data;
    data.length = 5;
    data.workerId = 0;
    data.counter = counter;
    data.type = expectedType;

    EXPECT_TRUE(coordinator.sendResponseQueue(data));

    EXPECT_EQ(coordinator.sendRequest(workerId, counter, type, expectedType),
              outpost::sip::OperationResult::workerIdError);
}

TEST_F(TestCoordinator, responseTypeError)
{
    const uint8_t workerId = 1;
    const uint8_t counter = 2;
    const uint8_t type = 3;
    const uint8_t expectedType = 4;

    outpost::sip::Coordinator::ResponseData data;
    data.length = 5;
    data.workerId = workerId;
    data.counter = counter;
    data.type = 0;

    EXPECT_TRUE(coordinator.sendResponseQueue(data));

    EXPECT_EQ(coordinator.sendRequest(workerId, counter, type, expectedType),
              outpost::sip::OperationResult::responseTypeError);
}

TEST_F(TestCoordinator, responseErrorWithData)
{
    uint8_t payloadWorkerData[1];
    outpost::Slice<uint8_t> workerData = outpost::asSlice(payloadWorkerData);
    EXPECT_EQ(coordinator.sendRequestGetResponseData(
                      1, 2, 3, 4, outpost::Slice<uint8_t>::empty(), workerData),
              outpost::sip::OperationResult::responseError);
}

TEST_F(TestCoordinator, successWithData)
{
    if (outpost::sip::parameter::maxPayloadLength < 4)
    {
        // this test requires payload data
        return;
    }
    const uint8_t workerId = 9;
    const uint8_t counter = 4;
    const uint8_t type = 9;
    const uint8_t expectedType = 4;
    serial.mDataToTransmit.clear();
    serial.mDataToReceive.clear();

    outpost::sip::Coordinator::ResponseData data;
    data.length = 5;
    data.workerId = workerId;
    data.counter = counter;
    data.type = expectedType;
    data.payloadData[0] = 55;
    data.payloadDataLength = 1;

    EXPECT_TRUE(coordinator.sendResponseQueue(data));

    uint8_t payloadData[4] = {0x01, 0x02, 0x03, 0x04};

    uint8_t payloadWorkerData[1];
    outpost::Slice<uint8_t> workerData = outpost::asSlice(payloadWorkerData);
    EXPECT_EQ(coordinator.sendRequestGetResponseData(workerId,
                                                     counter,
                                                     type,
                                                     expectedType,
                                                     outpost::asSlice(payloadData),
                                                     workerData),
              outpost::sip::OperationResult::success);

    EXPECT_EQ(data.payloadData[0], workerData[0]);

    // test the correct serialization
    EXPECT_EQ(serial.mDataToTransmit[0], 0x7e);
    EXPECT_EQ(serial.mDataToTransmit[1], 0x00);
    EXPECT_EQ(serial.mDataToTransmit[2], 0x09);
    EXPECT_EQ(serial.mDataToTransmit[3], workerId);
    EXPECT_EQ(serial.mDataToTransmit[4], counter);
    EXPECT_EQ(serial.mDataToTransmit[5], type);
    EXPECT_EQ(serial.mDataToTransmit[6], payloadData[0]);
    EXPECT_EQ(serial.mDataToTransmit[7], payloadData[1]);
    EXPECT_EQ(serial.mDataToTransmit[8], payloadData[2]);
    EXPECT_EQ(serial.mDataToTransmit[9], payloadData[3]);
    // EXPECT_EQ(serial.mDataToTransmit[10], 0xcc); // crc
    // EXPECT_EQ(serial.mDataToTransmit[11], 0x78); // crc
    EXPECT_EQ(serial.mDataToTransmit[12], 0x7e);
}

TEST_F(TestCoordinator, workerIdErrorWithData)
{
    if (outpost::sip::parameter::maxPayloadLength < 1)
    {
        // this test requires payload data
        return;
    }

    const uint8_t workerId = 1;
    const uint8_t counter = 2;
    const uint8_t type = 3;
    const uint8_t expectedType = 4;

    outpost::sip::Coordinator::ResponseData data;
    data.length = 5;
    data.workerId = 0;
    data.counter = counter;
    data.type = expectedType;
    data.payloadData[0] = 55;
    data.payloadDataLength = 1;

    EXPECT_TRUE(coordinator.sendResponseQueue(data));

    uint8_t payloadWorkerData[1];
    outpost::Slice<uint8_t> workerData = outpost::asSlice(payloadWorkerData);
    EXPECT_EQ(coordinator.sendRequestGetResponseData(workerId,
                                                     counter,
                                                     type,
                                                     expectedType,
                                                     outpost::Slice<uint8_t>::empty(),
                                                     workerData),
              outpost::sip::OperationResult::workerIdError);
}

TEST_F(TestCoordinator, responseTypeErrorWithData)
{
    if (outpost::sip::parameter::maxPayloadLength < 1)
    {
        // this test requires payload data
        return;
    }

    const uint8_t workerId = 1;
    const uint8_t counter = 2;
    const uint8_t type = 3;
    const uint8_t expectedType = 4;

    outpost::sip::Coordinator::ResponseData data;
    data.length = 5;
    data.workerId = workerId;
    data.counter = counter;
    data.type = 0;
    data.payloadData[0] = 55;
    data.payloadDataLength = 1;

    EXPECT_TRUE(coordinator.sendResponseQueue(data));

    uint8_t payloadWorkerData[1];
    outpost::Slice<uint8_t> workerData = outpost::asSlice(payloadWorkerData);
    EXPECT_EQ(coordinator.sendRequestGetResponseData(workerId,
                                                     counter,
                                                     type,
                                                     expectedType,
                                                     outpost::Slice<uint8_t>::empty(),
                                                     workerData),
              outpost::sip::OperationResult::responseTypeError);
}
