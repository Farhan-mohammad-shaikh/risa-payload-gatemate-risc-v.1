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

#include <outpost/transport/frame_coder/buffered_frame_decoder_hdlc.h>
#include <outpost/transport/frame_coder/frame_encoder_hdlc.h>
#include <outpost/utils/expected.h>

#include <unittest/harness.h>

class TestBufferedFrameDecoderHdlc : public ::testing::Test
{
public:
    TestBufferedFrameDecoderHdlc()
    {
    }

    constexpr static size_t mMaxPayloadSize = 128;
    constexpr static size_t mMaxFrameSize = 2 * mMaxPayloadSize + 2;
    std::array<uint8_t, mMaxFrameSize> mFrameDecoderBuffer;
    outpost::transport::BufferedFrameDecoderHdlc mFrameDecoder{
            outpost::asSlice(mFrameDecoderBuffer)};

    outpost::transport::FrameEncoderHdlc mFrameEncoder;

    outpost::Expected<outpost::Slice<uint8_t>, outpost::transport::OperationResult> result =
            outpost::unexpected(outpost::transport::OperationResult::decodingError);
    outpost::Slice<uint8_t> decoded = outpost::Slice<uint8_t>::empty();
};

TEST_F(TestBufferedFrameDecoderHdlc, encodeDecodeLoop)
{
    std::array<uint8_t, mMaxPayloadSize> inputBuffer;
    std::array<uint8_t, mMaxFrameSize> outputEncodeBuffer;
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

    ASSERT_TRUE(result);
    decoded = *result;
    ASSERT_TRUE(SliceMatch(outpost::asSlice(inputBuffer), decoded));
}

TEST_F(TestBufferedFrameDecoderHdlc, decodeSuccessWithGarbage)
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
    inputBuffer[i++] = 0x7E;
    inputBuffer[i++] = 42;
    inputBuffer[i++] = 0x7E;

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
    ASSERT_TRUE(result);
    decoded = *result;
    ASSERT_EQ(42, decoded[0]);
}

TEST_F(TestBufferedFrameDecoderHdlc, decodeFailWithoutEndMarker)
{
    std::array<uint8_t, 4> inputBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;

    inputBuffer.fill(0);
    inputBuffer[0] = 0x7E;
    inputBuffer[1] = 42;

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

TEST_F(TestBufferedFrameDecoderHdlc, decodeSuccessWithSharedBackToBackMarker)
{
    std::array<uint8_t, 256> inputBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;

    inputBuffer.fill(0);
    size_t i = 0;
    inputBuffer[i++] = 0x7E;
    inputBuffer[i++] = 42;
    inputBuffer[i++] = 0x7E;
    inputBuffer[i++] = 24;
    inputBuffer[i++] = 0x7E;

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
    ASSERT_TRUE(result);
    decoded = *result;
    ASSERT_EQ(inputBuffer[1], decoded[0]);

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
    ASSERT_EQ(inputBuffer[3], decoded[0]);
}

TEST_F(TestBufferedFrameDecoderHdlc, decodeSuccessWithUnsharedBackToBackMarker)
{
    std::array<uint8_t, 256> inputBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;

    inputBuffer.fill(0);
    size_t i = 0;
    inputBuffer[i++] = 0x7E;
    inputBuffer[i++] = 42;
    inputBuffer[i++] = 0x7E;
    inputBuffer[i++] = 0x7E;
    inputBuffer[i++] = 24;
    inputBuffer[i++] = 0x7E;

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
    ASSERT_TRUE(result);
    decoded = *result;
    ASSERT_EQ(inputBuffer[1], decoded[0]);

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
    ASSERT_EQ(inputBuffer[4], decoded[0]);
}

TEST_F(TestBufferedFrameDecoderHdlc, clearResetsState)
{
    std::array<uint8_t, 256> inputBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;

    inputBuffer.fill(0);
    size_t i = 0;
    inputBuffer[i++] = 0x7E;
    inputBuffer[i++] = 42;
    // clear occurs
    inputBuffer[i++] = 0x7E;
    inputBuffer[i++] = 24;
    inputBuffer[i++] = 0x7E;

    auto it = inputBuffer.begin();
    while (it != inputBuffer.end())
    {
        if (std::distance(inputBuffer.begin(), it) == 2)
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
    ASSERT_EQ(inputBuffer[3], decoded[0]);
}

TEST_F(TestBufferedFrameDecoderHdlc, decodeFailWhenFull)
{
    std::array<uint8_t, mMaxFrameSize + 1> inputBuffer;
    std::array<uint8_t, inputBuffer.size()> outputDecodeBuffer;

    inputBuffer.fill(0);
    inputBuffer[0] = 0x7E;

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
    ASSERT_EQ(result.error(), outpost::transport::OperationResult::bufferError);
}
