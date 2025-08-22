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

#include <outpost/transport/frame_coder/frame_decoder_hdlc.h>
#include <outpost/transport/frame_coder/frame_encoder_hdlc.h>

#include <unittest/harness.h>

class TestFrameDecoderHdlc : public ::testing::Test
{
public:
    TestFrameDecoderHdlc()
    {
    }

    outpost::transport::FrameDecoderHdlc mDecoder;
    outpost::transport::FrameEncoderHdlc mEncoder;
};

TEST_F(TestFrameDecoderHdlc, decode)
{
    std::array<uint8_t, 5> content{{0x00, 0x05, 1, 2, 3}};

    std::array<uint8_t, content.size() + 2> encodeBufferStorage;
    const auto maybeEncoded =
            mEncoder.encode(outpost::asSlice(content), outpost::asSlice(encodeBufferStorage));
    ASSERT_TRUE(maybeEncoded);
    const auto& encoded = *maybeEncoded;

    std::array<uint8_t, content.size()> decodeBuffer;

    const auto maybeDecoded = mDecoder.decode(encoded, outpost::asSlice(decodeBuffer));
    ASSERT_TRUE(maybeDecoded);
    ASSERT_TRUE(SliceMatch(*maybeDecoded, outpost::asSlice(content)));
}

TEST_F(TestFrameDecoderHdlc, decodeDelimiter)
{
    std::array<uint8_t, 2> content{{0x7E, 0x7E}};

    std::array<uint8_t, content.size() * 2 + 2> encodeBufferStorage;
    const auto maybeEncoded =
            mEncoder.encode(outpost::asSlice(content), outpost::asSlice(encodeBufferStorage));
    ASSERT_TRUE(maybeEncoded);
    const auto& encoded = *maybeEncoded;

    std::array<uint8_t, content.size()> decodeBuffer;

    const auto maybeDecoded = mDecoder.decode(encoded, outpost::asSlice(decodeBuffer));
    ASSERT_TRUE(maybeDecoded);
    ASSERT_TRUE(SliceMatch(*maybeDecoded, outpost::asSlice(content)));
}

TEST_F(TestFrameDecoderHdlc, decodeFail)
{
    std::array<uint8_t, 2> content{{0x7E, 0x7E}};

    std::array<uint8_t, content.size() * 2 + 2> encodeBufferStorage;
    const auto maybeEncoded =
            mEncoder.encode(outpost::asSlice(content), outpost::asSlice(encodeBufferStorage));
    ASSERT_TRUE(maybeEncoded);
    const auto& encoded = *maybeEncoded;

    encoded.last(1)[0] = 0x00;

    std::array<uint8_t, content.size()> decodeBuffer;

    const auto maybeDecoded = mDecoder.decode(encoded, outpost::asSlice(decodeBuffer));
    ASSERT_FALSE(maybeDecoded);
}
