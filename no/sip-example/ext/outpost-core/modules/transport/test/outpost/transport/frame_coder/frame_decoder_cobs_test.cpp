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

#include <outpost/transport/frame_coder/frame_decoder_cobs.h>
#include <outpost/transport/frame_coder/frame_encoder_cobs.h>

#include <unittest/harness.h>

class TestFrameDecoderCobs : public ::testing::Test
{
public:
    TestFrameDecoderCobs()
    {
    }

    class CobsFrameValidator
    {
    public:
        void
        validateEncoding(const outpost::Slice<const uint8_t>& input,
                         const outpost::Slice<const uint8_t>& output)
        {
            size_t inputIndex = 0;
            size_t outputIndex = 0;

            ASSERT_GT(output.getNumberOfElements(), input.getNumberOfElements());

            if (mDecodedFramesBeginWithDelimiter)
            {
                EXPECT_EQ(output[outputIndex++], outpost::coding::cobsFrameDelimiter);
            }

            // COBS code byte
            uint8_t dataBytesUntilCode = output[outputIndex++];
            bool nextCodeByteIsCoding = (dataBytesUntilCode != 255);

            while (inputIndex < input.getNumberOfElements())
            {
                ASSERT_TRUE(dataBytesUntilCode > 0);
                dataBytesUntilCode--;
                if (dataBytesUntilCode == 0)
                {
                    if (nextCodeByteIsCoding)
                    {
                        EXPECT_EQ(input[inputIndex++], outpost::coding::cobsFrameDelimiter);
                    }
                    dataBytesUntilCode = output[outputIndex++];
                    nextCodeByteIsCoding = (dataBytesUntilCode != 255);
                }
                else
                {
                    EXPECT_EQ(input[inputIndex++], output[outputIndex++]);
                }
            }

            ASSERT_EQ(outputIndex, output.getNumberOfElements() - 1);
            ASSERT_EQ(output[outputIndex++], outpost::coding::cobsFrameDelimiter);
        };

        bool mDecodedFramesBeginWithDelimiter = false;
    };

    outpost::transport::FrameDecoderCobs mDecoder;
    outpost::transport::FrameEncoderCobs mEncoder;
    CobsFrameValidator mValidator;
};

TEST_F(TestFrameDecoderCobs, decode)
{
    std::array<uint8_t, 256> content;
    outpost::transport::FrameEncoderCobs::WorseCaseSizedBuffer<content.size()> encodeBuffer;

    const auto maybeEncoded =
            mEncoder.encode(outpost::asSlice(content), outpost::asSlice(encodeBuffer));
    ASSERT_TRUE(maybeEncoded);
    const auto& encoded = *maybeEncoded;

    outpost::transport::FrameDecoderCobs::WorseCaseSizedBuffer<content.size()> decodeBuffer;

    const auto maybeDecoded = mDecoder.decode(encoded, outpost::asSlice(decodeBuffer));
    ASSERT_TRUE(maybeDecoded);
    ASSERT_TRUE(SliceMatch(*maybeDecoded, outpost::asSlice(content)));
}

TEST_F(TestFrameDecoderCobs, decodeFail)
{
    std::array<uint8_t, 256> content;
    outpost::transport::FrameEncoderCobs::WorseCaseSizedBuffer<content.size()> encodeBuffer;

    const auto maybeEncoded =
            mEncoder.encode(outpost::asSlice(content), outpost::asSlice(encodeBuffer));
    ASSERT_TRUE(maybeEncoded);
    const auto& encoded = *maybeEncoded;

    encoded.first(1)[0] = 0x00;

    outpost::transport::FrameDecoderCobs::WorseCaseSizedBuffer<content.size()> decodeBuffer;
    const auto maybeDecoded = mDecoder.decode(encoded, outpost::asSlice(decodeBuffer));
    ASSERT_FALSE(maybeDecoded);
}
