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

#include <outpost/transport/frame_coder/frame_encoder_hdlc.h>

#include <unittest/harness.h>

class TestFrameEncoderHdlc : public ::testing::Test
{
public:
    TestFrameEncoderHdlc()
    {
    }

    outpost::transport::FrameEncoderHdlc mEncoder;
};

TEST_F(TestFrameEncoderHdlc, encode)
{
    std::array<uint8_t, 5> content{{0x00, 0x05, 1, 2, 3}};

    outpost::transport::FrameEncoderHdlc::WorseCaseSizedBuffer<content.size()> encodeBufferStorage;
    const auto maybeEncoded =
            mEncoder.encode(outpost::asSlice(content), outpost::asSlice(encodeBufferStorage));
    ASSERT_TRUE(maybeEncoded);
    const auto& encoded = *maybeEncoded;

    EXPECT_EQ(encoded[0], 0x7E);  // boundary
    EXPECT_EQ(encoded[1], 0x00);
    EXPECT_EQ(encoded[2], 0x05);
    ASSERT_EQ(encoded[3], 1);
    ASSERT_EQ(encoded[4], 2);
    ASSERT_EQ(encoded[5], 3);
    EXPECT_EQ(encoded[6], 0x7E);  // boundary
}

TEST_F(TestFrameEncoderHdlc, encodeDelimiter)
{
    std::array<uint8_t, 2> content{{0x7E, 0x7E}};

    outpost::transport::FrameEncoderHdlc::WorseCaseSizedBuffer<content.size()> encodeBufferStorage;
    const auto maybeEncoded =
            mEncoder.encode(outpost::asSlice(content), outpost::asSlice(encodeBufferStorage));
    ASSERT_TRUE(maybeEncoded);
    const auto& encoded = *maybeEncoded;

    EXPECT_EQ(encoded[0], 0x7E);  // boundary
    EXPECT_EQ(encoded[1], 0x7D);
    EXPECT_EQ(encoded[2], 0x5E);
    EXPECT_EQ(encoded[3], 0x7D);
    EXPECT_EQ(encoded[4], 0x5E);
    EXPECT_EQ(encoded[5], 0x7E);  // boundary
}
