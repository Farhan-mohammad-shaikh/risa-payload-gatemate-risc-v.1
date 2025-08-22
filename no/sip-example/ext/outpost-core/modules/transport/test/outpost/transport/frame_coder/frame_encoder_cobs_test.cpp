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

#include <outpost/transport/frame_coder/frame_encoder_cobs.h>

#include <unittest/harness.h>

class TestFrameEncoderCobs : public ::testing::Test
{
public:
    TestFrameEncoderCobs()
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

            if (mEncodedFramesBeginWithDelimiter)
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

        bool mEncodedFramesBeginWithDelimiter = false;
    };

    outpost::transport::FrameEncoderCobs mEncoder;
    CobsFrameValidator mValidator;
};

TEST_F(TestFrameEncoderCobs, encode)
{
    std::array<uint8_t, 256> content;
    outpost::transport::FrameEncoderCobs::WorseCaseSizedBuffer<content.size()> encodeBufferStorage;

    const auto maybeEncoded =
            mEncoder.encode(outpost::asSlice(content), outpost::asSlice(encodeBufferStorage));
    ASSERT_TRUE(maybeEncoded);
    const auto& encoded = *maybeEncoded;

    mValidator.validateEncoding(outpost::asSlice(content), encoded);
}
