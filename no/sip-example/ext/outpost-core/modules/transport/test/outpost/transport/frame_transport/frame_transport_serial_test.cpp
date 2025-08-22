/*
 * Copyright (c) 2024, Cold, Erin Cynthia
 * Copyright (c) 2024, Tepe, Alexander
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <harness/hal/serial_mock.h>

#include <outpost/rtos.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder_cobs.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder_hdlc.h>
#include <outpost/transport/frame_coder/frame_encoder_cobs.h>
#include <outpost/transport/frame_coder/frame_encoder_hdlc.h>
#include <outpost/transport/frame_transport/frame_transport_serial.h>

#include <unittest/hal/serial_stub.h>
#include <unittest/harness.h>

#include <vector>

using namespace ::testing;

using HdclCoderTypes = std::pair<outpost::transport::FrameEncoderHdlc,
                                 outpost::transport::BufferedFrameDecoderHdlc>;
using CobsCoderTypes = std::pair<outpost::transport::FrameEncoderCobs,
                                 outpost::transport::BufferedFrameDecoderCobs>;
using FrameEncoderTypes = ::testing::Types<HdclCoderTypes, CobsCoderTypes>;

constexpr static size_t maxPayloadSize = 128;
constexpr static size_t maxFrameSize = 2 * maxPayloadSize + 2;

template <typename FrameEncoderTypes>
class TestFrameTransportSerial : public ::testing::Test
{
public:
    TestFrameTransportSerial() :
        bufferedFrameDecoder(outpost::asSlice(decodeBuffer)),
        frameTransportTxSerial(serialStub, frameEncoder, outpost::asSlice(transmitBuffer)),
        frameTransportRxSerial(clock, serialStub, bufferedFrameDecoder, /* clearOnTimeout = */ true)
    {
    }

    outpost::rtos::SystemClock clock;

    unittest::hal::SerialStub serialStub;
    harness::hal::SerialMock serialMock;

    typename FrameEncoderTypes::first_type frameEncoder;
    typename FrameEncoderTypes::second_type bufferedFrameDecoder;

    std::array<uint8_t, maxFrameSize> decodeBuffer;
    std::array<uint8_t, maxFrameSize> transmitBuffer;
    outpost::transport::FrameTransportTxSerial frameTransportTxSerial;
    outpost::transport::FrameTransportRxSerial frameTransportRxSerial;

    size_t
    getNeededBytesWhenEncoded(const outpost::Slice<const uint8_t>& thing)
    {
        std::vector<uint8_t> buffer(thing.getNumberOfElements() * 2 + 2);
        auto inout = outpost::asSlice(buffer);
        auto maybeFrame = frameEncoder.encode(thing, inout);
        if (!maybeFrame)
        {
            std::cout << "Could not encode frame" << std::endl;
            return 0;
        }

        const size_t encodedLength = maybeFrame->getNumberOfElements();
        return encodedLength;
    }

    void
    setupSerialMockWithTimeoutHalfway(const outpost::Slice<const uint8_t>& payload)
    {
        // Encode the payload
        static std::array<uint8_t, maxFrameSize> codedArray;  // static so slices are still valid
                                                              // when expectations are tested
        auto maybeFrame =
                frameEncoder.encode(outpost::asSlice(payload), outpost::asSlice(codedArray));
        ASSERT_TRUE(maybeFrame);
        auto frame = *maybeFrame;

        // Reading from serial will time out halfway, resume with the rest of the message, and then
        // stop
        const size_t timeoutPoint = frame.getNumberOfElements() / 2;
        {
            ::testing::InSequence seq;
            for (size_t i = 0; i < timeoutPoint; ++i)
            {
                EXPECT_CALL(this->serialMock, read(_, _))
                        .WillOnce(harness::hal::SerialMock::ReadFrom(frame.subSlice(i, 1)));
            }
            EXPECT_CALL(this->serialMock, read(_, _))
                    .WillOnce(
                            harness::hal::SerialMock::ReadFrom((outpost::Slice<uint8_t>::empty())));
            for (size_t i = timeoutPoint; i < frame.getNumberOfElements(); ++i)
            {
                EXPECT_CALL(this->serialMock, read(_, _))
                        .WillOnce(harness::hal::SerialMock::ReadFrom(frame.subSlice(i, 1)));
            }
            EXPECT_CALL(this->serialMock, read(_, _))
                    .WillRepeatedly(
                            harness::hal::SerialMock::ReadFrom((outpost::Slice<uint8_t>::empty())));
        }
    }
};

// cppcheck-suppress syntaxError
TYPED_TEST_SUITE(TestFrameTransportSerial, FrameEncoderTypes, /* NameGenerator */);

TYPED_TEST(TestFrameTransportSerial, transmit)
{
    this->serialStub.mDataToTransmit.clear();
    std::array<uint8_t, 2> payload = {55, 66};

    const auto maybeNumTransmittedBytes =
            this->frameTransportTxSerial.transmit(outpost::asSlice(payload));
    ASSERT_TRUE(maybeNumTransmittedBytes)
            << " Could not transmit bytes because "
            << testing::PrintToString(maybeNumTransmittedBytes.error());

    const auto expectedNumBytes = this->getNeededBytesWhenEncoded(outpost::asSlice(payload));
    ASSERT_EQ(*maybeNumTransmittedBytes, expectedNumBytes);
    EXPECT_EQ(this->serialStub.mDataToTransmit.size(), *maybeNumTransmittedBytes);

    std::array<uint8_t, maxFrameSize> codedArray;
    auto maybeFrame =
            this->frameEncoder.encode(outpost::asSlice(payload), outpost::asSlice(codedArray));
    ASSERT_TRUE(maybeFrame);
    ASSERT_TRUE(SliceMatch(outpost::asSlice(this->serialStub.mDataToTransmit), *maybeFrame));
}

TYPED_TEST(TestFrameTransportSerial, receive)
{
    std::array<uint8_t, 2> payload = {55, 66};
    std::array<uint8_t, maxFrameSize> codedArray;

    auto maybeFrame =
            this->frameEncoder.encode(outpost::asSlice(payload), outpost::asSlice(codedArray));
    ASSERT_TRUE(maybeFrame);

    // Insert data
    this->serialStub.mDataToReceive.insert(
            this->serialStub.mDataToReceive.end(), maybeFrame->begin(), maybeFrame->end());

    std::array<uint8_t, maxFrameSize> bufferRet;
    auto maybeBytes = this->frameTransportRxSerial.receive(outpost::asSlice(bufferRet),
                                                           outpost::time::Milliseconds(10));

    ASSERT_TRUE(maybeBytes);
    ASSERT_TRUE(SliceMatch(*maybeBytes, outpost::asSlice(payload)));
}

TYPED_TEST(TestFrameTransportSerial, receiveTimeout)
{
    this->serialStub.mDataToReceive.clear();

    std::array<uint8_t, maxFrameSize> buffer;
    auto const result = this->frameTransportRxSerial.receive(outpost::asSlice(buffer),
                                                             outpost::time::Milliseconds(100));

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), outpost::transport::OperationResult::timeout);
}

TYPED_TEST(TestFrameTransportSerial, transmitHDLCFlag)
{
    std::array<uint8_t, maxPayloadSize> payload;
    std::fill(payload.begin(), payload.end(), 0x7E);

    const auto maybeNumTransmittedBytes =
            this->frameTransportTxSerial.transmit(outpost::asSlice(payload));
    ASSERT_TRUE(maybeNumTransmittedBytes);

    const auto expectedNumBytes = this->getNeededBytesWhenEncoded(outpost::asSlice(payload));
    ASSERT_EQ(*maybeNumTransmittedBytes, expectedNumBytes);
    EXPECT_EQ(this->serialStub.mDataToTransmit.size(), *maybeNumTransmittedBytes);

    std::array<uint8_t, maxFrameSize> codedArray;
    auto maybeFrame =
            this->frameEncoder.encode(outpost::asSlice(payload), outpost::asSlice(codedArray));
    ASSERT_TRUE(maybeFrame);
    ASSERT_TRUE(SliceMatch(outpost::asSlice(this->serialStub.mDataToTransmit), *maybeFrame));
}

TYPED_TEST(TestFrameTransportSerial, transmitFrameTooBig)
{
    std::array<uint8_t, maxFrameSize + 1> payload;

    const auto maybeTransmittedBytes =
            this->frameTransportTxSerial.transmit(outpost::asSlice(payload));

    ASSERT_FALSE(maybeTransmittedBytes);
}

TYPED_TEST(TestFrameTransportSerial, receiveAllowedButTooBigPayload)
{
    uint8_t payload[] = {"Hello, World!"};

    const auto maybeTransmittedBytes =
            this->frameTransportTxSerial.transmit(outpost::asSlice(payload));
    ASSERT_TRUE(maybeTransmittedBytes);

    this->serialStub.mDataToReceive = this->serialStub.mDataToTransmit;

    // This is intentionally too small for the frame
    std::array<uint8_t, 13> bufferRet;

    auto maybeBytes = this->frameTransportRxSerial.receive(outpost::asSlice(bufferRet),
                                                           outpost::time::Milliseconds(10));

    ASSERT_FALSE(maybeBytes);
    EXPECT_EQ(maybeBytes.error(), outpost::transport::OperationResult::decodingError);
}

TYPED_TEST(TestFrameTransportSerial, receiveTimeoutClearsBufferWhenTrue)
{
    outpost::transport::FrameTransportRxSerial frameTransportThatClears(
            this->clock,
            this->serialMock,
            this->bufferedFrameDecoder,
            /* clearOnTimeout = */ true);

    std::array<uint8_t, 2> payload = {55, 66};

    this->setupSerialMockWithTimeoutHalfway(outpost::asSlice(payload));

    // First Receive times out
    std::array<uint8_t, maxFrameSize> bufferRet;

    auto maybeBytes = frameTransportThatClears.receive(outpost::asSlice(bufferRet),
                                                       outpost::time::Milliseconds(10));
    ASSERT_FALSE(maybeBytes);
    EXPECT_EQ(maybeBytes.error(), outpost::transport::OperationResult::timeout);

    // Second Receive fails since the decode buffer was cleared
    maybeBytes = frameTransportThatClears.receive(outpost::asSlice(bufferRet),
                                                  outpost::time::Milliseconds(10));
    ASSERT_FALSE(maybeBytes);
}

TYPED_TEST(TestFrameTransportSerial, receiveTimeoutRetainsBufferWhenFalse)
{
    outpost::transport::FrameTransportRxSerial frameTransportThatRetains(
            this->clock,
            this->serialMock,
            this->bufferedFrameDecoder,
            /* clearOnTimeout = */ false);

    std::array<uint8_t, 2> payload = {55, 66};

    this->setupSerialMockWithTimeoutHalfway(outpost::asSlice(payload));

    // First Receive times out
    std::array<uint8_t, maxFrameSize> bufferRet;

    auto maybeBytes = frameTransportThatRetains.receive(outpost::asSlice(bufferRet),
                                                        outpost::time::Milliseconds(10));
    ASSERT_FALSE(maybeBytes);
    EXPECT_EQ(maybeBytes.error(), outpost::transport::OperationResult::timeout);

    // Second Receive succeeds since the decode buffer was cleared
    maybeBytes = frameTransportThatRetains.receive(outpost::asSlice(bufferRet),
                                                   outpost::time::Milliseconds(10));
    ASSERT_TRUE(maybeBytes);
    ASSERT_TRUE(SliceMatch(outpost::asSlice(payload), *maybeBytes));
}

TYPED_TEST(TestFrameTransportSerial, serialTxDisconnect)
{
    outpost::transport::FrameTransportTxSerial frameTransportTx(
            this->serialMock, this->frameEncoder, outpost::asSlice(this->transmitBuffer));
    std::array<uint8_t, 2> buffer = {55, 66};

    EXPECT_CALL(this->serialMock, write(_, _))
            .WillOnce(Return(outpost::unexpected(outpost::hal::Serial::SerialError::error())));

    const auto maybeTransmittedBytes = frameTransportTx.transmit(outpost::asSlice(buffer));
    ASSERT_FALSE(maybeTransmittedBytes);
    EXPECT_EQ(maybeTransmittedBytes.error(), outpost::transport::OperationResult::streamStopped);
}

TYPED_TEST(TestFrameTransportSerial, serialRxDisconnect)
{
    outpost::transport::FrameTransportRxSerial frameTransportRx(
            this->clock, this->serialMock, this->bufferedFrameDecoder);
    std::array<uint8_t, 2> buffer = {55, 66};

    EXPECT_CALL(this->serialMock, read(_, _))
            .WillOnce(Return(outpost::unexpected(outpost::hal::Serial::SerialError::error())));

    auto maybeBytes =
            frameTransportRx.receive(outpost::asSlice(buffer), outpost::time::Milliseconds(10));

    ASSERT_FALSE(maybeBytes);
    EXPECT_EQ(maybeBytes.error(), outpost::transport::OperationResult::streamStopped);
}

TYPED_TEST(TestFrameTransportSerial, combinedLoopbackMultiplePacket)
{
    outpost::transport::FrameTransportSerial frameTransport(this->clock,
                                                            this->serialStub,
                                                            this->frameEncoder,
                                                            outpost::asSlice(this->transmitBuffer),
                                                            this->bufferedFrameDecoder);

    std::array<std::vector<uint8_t>, 3> payloads = {{{0x00}, {1, 2}, {0xFF, 0xA5, 0xFF}}};

    for (auto payload : payloads)
    {
        const auto maybeNumTransmittedBytes =
                this->frameTransportTxSerial.transmit(outpost::asSlice(payload));
        ASSERT_TRUE(maybeNumTransmittedBytes);
    }

    this->serialStub.mDataToReceive = this->serialStub.mDataToTransmit;

    std::array<uint8_t, maxPayloadSize> bufferRet;
    for (auto payload : payloads)
    {
        const auto maybeBytes = this->frameTransportRxSerial.receive(
                outpost::asSlice(bufferRet), outpost::time::Milliseconds(10));

        ASSERT_TRUE(maybeBytes);
        ASSERT_TRUE(SliceMatch(*maybeBytes, outpost::asSlice(payload)));
    }
}

TYPED_TEST(TestFrameTransportSerial, zeroDurationShouldConstructFrame)
{
    std::array<uint8_t, 2> payload = {55, 66};
    std::array<uint8_t, maxFrameSize> codedArray;

    auto const maybeFrame =
            this->frameEncoder.encode(outpost::asSlice(payload), outpost::asSlice(codedArray));
    ASSERT_TRUE(maybeFrame);

    // Insert data
    this->serialStub.mDataToReceive.insert(
            this->serialStub.mDataToReceive.end(), maybeFrame->begin(), maybeFrame->end());

    std::array<uint8_t, maxFrameSize> bufferRet;
    auto const maybeBytes = this->frameTransportRxSerial.receive(outpost::asSlice(bufferRet),
                                                                 outpost::time::Duration::zero());

    ASSERT_TRUE(maybeBytes);
    EXPECT_TRUE(SliceMatch(*maybeBytes, outpost::asSlice(payload)));
}

TYPED_TEST(TestFrameTransportSerial, zeroDurationShouldTimeoutOnEmptyBuffer)
{
    this->serialStub.mDataToReceive.clear();

    std::array<uint8_t, maxFrameSize> buffer;
    auto const result = this->frameTransportRxSerial.receive(outpost::asSlice(buffer),
                                                             outpost::time::Duration::zero());

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), outpost::transport::OperationResult::timeout);
}
