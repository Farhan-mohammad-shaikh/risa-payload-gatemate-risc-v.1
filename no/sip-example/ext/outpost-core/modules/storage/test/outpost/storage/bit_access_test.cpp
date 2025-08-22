/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/storage/bit_access.h>

#include <unittest/harness.h>

using outpost::BitAccess;

class BitAccessTest : public testing::Test
{
};

TEST_F(BitAccessTest, accessSingleBits)
{
    uint32_t reference = 0x12345678;
    bool bit;

    bit = BitAccess::get<uint32_t, 0>(reference);
    EXPECT_FALSE(bit);

    bit = BitAccess::get<uint32_t, 1>(reference);
    EXPECT_FALSE(bit);

    bit = BitAccess::get<uint32_t, 2>(reference);
    EXPECT_FALSE(bit);

    bit = BitAccess::get<uint32_t, 3>(reference);
    EXPECT_TRUE(bit);

    bit = BitAccess::get<uint32_t, 16>(reference);
    EXPECT_FALSE(bit);

    bit = BitAccess::get<uint32_t, 18>(reference);
    EXPECT_TRUE(bit);

    bit = BitAccess::get<uint32_t, 28>(reference);
    EXPECT_TRUE(bit);
}

TEST_F(BitAccessTest, accessMultipleBits)
{
    uint32_t reference = 0x12345678;
    uint32_t value;

    value = BitAccess::get<uint32_t, 31, 0>(reference);
    EXPECT_EQ(0x12345678U, value);

    value = BitAccess::get<uint32_t, 2, 0>(reference);
    EXPECT_EQ(0x0U, value);

    value = BitAccess::get<uint32_t, 3, 0>(reference);
    EXPECT_EQ(0x8U, value);

    value = BitAccess::get<uint32_t, 7, 4>(reference);
    EXPECT_EQ(0x7U, value);

    value = BitAccess::get<uint32_t, 23, 16>(reference);
    EXPECT_EQ(0x34U, value);

    value = BitAccess::get<uint32_t, 19, 18>(reference);
    EXPECT_EQ(0x1U, value);

    value = BitAccess::get<uint32_t, 31, 28>(reference);
    EXPECT_EQ(0x1U, value);

    value = BitAccess::get<uint32_t, 31, 24>(reference);
    EXPECT_EQ(0x12U, value);
}

TEST_F(BitAccessTest, writeSingleBit1)
{
    uint32_t value = 0x12340000;
    BitAccess::set<uint32_t, 0>(value, true);
    EXPECT_EQ(0x12340001U, value);
}

TEST_F(BitAccessTest, writeSingleBit2)
{
    uint32_t value = 0x12340000;
    BitAccess::set<uint32_t, 5>(value, true);
    EXPECT_EQ(0x12340020U, value);
}

TEST_F(BitAccessTest, writeSingleBit3)
{
    uint32_t value = 0x12340000;
    BitAccess::set<uint32_t, 5>(value, false);
    EXPECT_EQ(0x12340000U, value);
}

TEST_F(BitAccessTest, writeSingleBit4)
{
    uint32_t value = 0x12340000;
    BitAccess::set<uint32_t, 28>(value, false);
    EXPECT_EQ(0x02340000U, value);
}

TEST_F(BitAccessTest, writeMultipleBitsFullAccess)
{
    uint32_t value = 0;
    BitAccess::set<uint32_t, 31, 0>(value, 0x12345678);
    EXPECT_EQ(0x12345678U, value);
}

TEST_F(BitAccessTest, writeSingleBitByMultipleBitAccess)
{
    uint32_t value = 0;
    BitAccess::set<uint32_t, 0, 0>(value, 1);
    EXPECT_EQ(0x1U, value);
}

TEST_F(BitAccessTest, writeMultipleBits2)
{
    uint32_t value = 0;
    BitAccess::set<uint32_t, 2, 0>(value, 2);
    EXPECT_EQ(0x2U, value);
}

TEST_F(BitAccessTest, writeMultipleBitsFull16bitAccess)
{
    uint32_t value = 0x12345678;
    BitAccess::set<uint32_t, 31, 16>(value, 0xABCD);
    EXPECT_EQ(0xABCD5678U, value);
}

TEST_F(BitAccessTest, writeMultipleBits4)
{
    uint32_t value = 0x12345678;
    BitAccess::set<uint32_t, 25, 19>(value, 0x13);
    EXPECT_EQ(0x109C5678U, value);
}

TEST_F(BitAccessTest, writeMultipleBitsWithAOverlongPattern)
{
    uint32_t value = 0x12345678;
    // Pattern is truncated to length specified through start and end of bitfield
    BitAccess::set<uint32_t, 25, 19>(value, 0x12313);
    EXPECT_EQ(0x109C5678U, value);
}
