/*
 * Copyright (c) 2014-2018, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Passenberg, Felix Constantin
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "outpost/base/slice.h"
#include <outpost/coding/cobs.h>

#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>

#include <unittest/harness.h>

#include <string.h>  // for memset

using outpost::coding::Cobs;
using ::testing::ElementsAreArray;

class CobsRoundtriptTest : public ::testing::Test
{
public:
    virtual void
    SetUp()
    {
        encoded.fill(0xAB);
        actual.fill(0xAC);
        enc = outpost::Slice<uint8_t>(encoded);
        act = outpost::Slice<uint8_t>(actual);
    }

    size_t
    roundtrip(outpost::Slice<uint8_t> input)
    {
        size_t encodedLength = Cobs::encode(input, enc);
        size_t decodedLength = Cobs::decode(enc, act);

        static_cast<void>(encodedLength);
        return decodedLength;
    }

    std::array<uint8_t, 1024> encoded;
    std::array<uint8_t, 1024> actual;
    outpost::Slice<uint8_t> enc{{}};
    outpost::Slice<uint8_t> act{{}};
};

// ----------------------------------------------------------------------------
TEST_F(CobsRoundtriptTest, singleZeroByte)
{
    uint8_t input[1] = {0};

    size_t decodedLength = roundtrip(outpost::asSlice(input));

    EXPECT_EQ(sizeof(input), decodedLength);
    EXPECT_THAT(input, ElementsAreArray(act.getDataPointer(), act.getNumberOfElements()));
}

TEST_F(CobsRoundtriptTest, zeroPrefixAndSuffix)
{
    uint8_t input[3] = {0, 1, 0};

    size_t decodedLength = roundtrip(outpost::asSlice(input));

    EXPECT_EQ(sizeof(input), decodedLength);
    EXPECT_THAT(input, ElementsAreArray(act.getDataPointer(), act.getNumberOfElements()));
}

TEST_F(CobsRoundtriptTest, blockOfDataWithoutZero)
{
    uint8_t input[512];
    for (size_t i = 0; i < 512; ++i)
    {
        input[i] = (i % 255) + 1;
    }

    size_t decodedLength = roundtrip(outpost::asSlice(input));

    EXPECT_EQ(sizeof(input), decodedLength);
    EXPECT_THAT(input, ElementsAreArray(act.getDataPointer(), act.getNumberOfElements()));
}

// ----------------------------------------------------------------------------
/*
 * Decoded the COBS data in place by using the same array for input and
 * output of the encoder.
 */
TEST_F(CobsRoundtriptTest, inPlaceDecodingOfBlockOfDataWithoutZero)
{
    std::array<uint8_t, 512> input;
    for (size_t i = 0; i < 512; ++i)
    {
        input[i] = (i % 255) + 1;
    }
    outpost::Slice<uint8_t> in(input);

    size_t encodedLength = Cobs::encode(in, enc);
    size_t decodedLength = Cobs::decode(enc, enc);

    EXPECT_EQ(input.size() + 3, encodedLength);
    EXPECT_EQ(input.size(), decodedLength);
    EXPECT_THAT(input, ElementsAreArray(enc.getDataPointer(), enc.getNumberOfElements()));
}

TEST_F(CobsRoundtriptTest, inPlaceDecodingOfZeroPrefixAndSuffix)
{
    std::array<uint8_t, 3> input = {0, 1, 0};
    outpost::Slice<uint8_t> in(input);

    size_t encodedLength = Cobs::encode(in, enc);
    size_t decodedLength = Cobs::decode(enc, enc);

    EXPECT_EQ(input.size() + 1, encodedLength);
    EXPECT_EQ(sizeof(input), decodedLength);
    EXPECT_THAT(input, ElementsAreArray(enc.getDataPointer(), enc.getNumberOfElements()));
}

RC_GTEST_FIXTURE_PROP(CobsRoundtriptTest, shouldPerformRoundTripWithRandomData, ())
{
    const auto input = *rc::gen::resize(200, rc::gen::arbitrary<std::vector<uint8_t>>());

    outpost::Slice<const uint8_t> inputArray(input);
    outpost::coding::CobsBase<32>::encode(inputArray, enc);
    size_t decodedLength = outpost::coding::CobsBase<32>::decode(enc, act);

    RC_ASSERT(input.size() == decodedLength);

    for (size_t i = 0; i < act.getNumberOfElements(); i++)
    {
        RC_ASSERT(input[i] == act[i]);
    }
}
