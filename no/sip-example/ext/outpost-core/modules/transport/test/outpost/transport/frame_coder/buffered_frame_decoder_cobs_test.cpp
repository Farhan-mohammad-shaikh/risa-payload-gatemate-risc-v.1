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

#include <outpost/transport/frame_coder/buffered_frame_decoder_cobs.h>
#include <outpost/transport/frame_coder/frame_encoder_cobs.h>
#include <outpost/utils/expected.h>

#include <unittest/harness.h>

#include <array>

class TestBufferedFrameDecoderCobs : public ::testing::Test
{
public:
    TestBufferedFrameDecoderCobs()
    {
    }

    constexpr static size_t mMaxFrameSize = 128;
    outpost::transport::BufferedFrameDecoderCobs::WorseCaseSizedBuffer<mMaxFrameSize>
            mFrameDecoderBuffer;
    outpost::transport::BufferedFrameDecoderCobs mFrameDecoder{
            outpost::asSlice(mFrameDecoderBuffer)};

    outpost::transport::FrameEncoderCobs mFrameEncoder;

    outpost::Expected<outpost::Slice<uint8_t>, outpost::transport::OperationResult> result =
            outpost::unexpected(outpost::transport::OperationResult::decodingError);
    outpost::Slice<uint8_t> decoded = outpost::Slice<uint8_t>::empty();
};

TEST_F(TestBufferedFrameDecoderCobs, encodeDecodeLoop)
{
    std::array<uint8_t, 256> inputBuffer;
    outpost::transport::FrameEncoderCobs::WorseCaseSizedBuffer<inputBuffer.size()>
            outputEncodeBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;
    inputBuffer.fill(42);

    const auto maybeEncoded = mFrameEncoder.encode(outpost::asSlice(inputBuffer),
                                                   outpost::asSlice(outputEncodeBuffer));
    ASSERT_TRUE(maybeEncoded);

    auto it = outputEncodeBuffer.begin();
    while (it != outputEncodeBuffer.end())
    {
        result = mFrameDecoder.bufferedDecode(*it, outpost::asSlice(outputDecodeBuffer));
        it++;
        if (result || (result.error() != outpost::transport::OperationResult::notComplete))
        {
            break;
        }
    }

    if (mFrameDecoderBuffer.size() < maybeEncoded->getNumberOfElements())
    {
        return;
    }
    ASSERT_TRUE(result);
    decoded = *result;
    ASSERT_TRUE(SliceMatch(outpost::asSlice(inputBuffer), decoded));
}

TEST_F(TestBufferedFrameDecoderCobs, decodeSuccessWithClearedGarbage)
{
    std::array<uint8_t, 256> inputBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;

    inputBuffer.fill(0);
    size_t i = 0;
    inputBuffer[i++] = 't';
    inputBuffer[i++] = 'r';
    inputBuffer[i++] = 'a';
    inputBuffer[i++] = 's';
    inputBuffer[i++] = 'h';
    inputBuffer[i++] = outpost::coding::cobsFrameDelimiter;  // clear
    inputBuffer[i++] = 2;
    inputBuffer[i++] = 42;
    inputBuffer[i++] = outpost::coding::cobsFrameDelimiter;

    auto it = inputBuffer.begin();
    while (it != inputBuffer.end())
    {
        result = mFrameDecoder.bufferedDecode(*it, outpost::asSlice(outputDecodeBuffer));
        it++;
        if (result || (result.error() != outpost::transport::OperationResult::notComplete))
        {
            break;
        }
    }
    ASSERT_FALSE(result);
    ASSERT_EQ(result.error(), outpost::transport::OperationResult::decodingError);

    while (it != inputBuffer.end())
    {
        result = mFrameDecoder.bufferedDecode(*it, outpost::asSlice(outputDecodeBuffer));
        it++;
        if (result || (result.error() != outpost::transport::OperationResult::notComplete))
        {
            break;
        }
    }
    ASSERT_TRUE(result);
    decoded = *result;
    ASSERT_EQ(42, decoded[0]);
}

TEST_F(TestBufferedFrameDecoderCobs, decodeFailWithoutEndMarker)
{
    std::array<uint8_t, 4> inputBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;

    inputBuffer.fill(42);

    auto it = inputBuffer.begin();
    while (it != inputBuffer.end())
    {
        result = mFrameDecoder.bufferedDecode(*it, outpost::asSlice(outputDecodeBuffer));
        it++;
        if (result || (result.error() != outpost::transport::OperationResult::notComplete))
        {
            break;
        }
    }
    ASSERT_FALSE(result);
    ASSERT_EQ(result.error(), outpost::transport::OperationResult::notComplete);
}

TEST_F(TestBufferedFrameDecoderCobs, clearResetsState)
{
    std::array<uint8_t, 256> inputBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;

    inputBuffer.fill(0);
    size_t i = 0;
    inputBuffer[i++] = 't';
    inputBuffer[i++] = 'r';
    inputBuffer[i++] = 'a';
    inputBuffer[i++] = 's';
    inputBuffer[i++] = 'h';
    // clear
    inputBuffer[i++] = 2;
    inputBuffer[i++] = 42;
    inputBuffer[i++] = outpost::coding::cobsFrameDelimiter;

    auto it = inputBuffer.begin();
    while (it != inputBuffer.end())
    {
        if (std::distance(inputBuffer.begin(), it) == 5)
        {
            mFrameDecoder.reset();
        }
        result = mFrameDecoder.bufferedDecode(*it, outpost::asSlice(outputDecodeBuffer));
        it++;
        if (result || (result.error() != outpost::transport::OperationResult::notComplete))
        {
            break;
        }
    }
    ASSERT_TRUE(result);
    decoded = *result;
    ASSERT_EQ(42, decoded[0]);
}

TEST_F(TestBufferedFrameDecoderCobs, decodeFailWhenFull)
{
    std::array<uint8_t, 1000> inputBuffer;
    std::array<uint8_t,
               outpost::coding::CobsFrame::getMaximumSizeOfEncodedFrame(inputBuffer.size())>
            outputEncodeBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;
    inputBuffer.fill(42);

    const auto maybeEncoded = mFrameEncoder.encode(outpost::asSlice(inputBuffer),
                                                   outpost::asSlice(outputEncodeBuffer));
    ASSERT_TRUE(maybeEncoded);

    auto it = outputEncodeBuffer.begin();
    while (it != outputEncodeBuffer.end())
    {
        result = mFrameDecoder.bufferedDecode(*it, outpost::asSlice(outputDecodeBuffer));
        it++;
        if (result || (result.error() != outpost::transport::OperationResult::notComplete))
        {
            break;
        }
    }
    ASSERT_FALSE(result);
    ASSERT_EQ(result.error(), outpost::transport::OperationResult::bufferError);
}
