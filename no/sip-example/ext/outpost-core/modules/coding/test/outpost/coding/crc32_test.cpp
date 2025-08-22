/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2024, Felix Passenberg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

/**
 * \file
 * \brief   Test CRC-32 functions
 *
 * See https://rosettacode.org/wiki/CRC-32#C.2B.2B for test pattern
 *
 * \author  Fabian Greif
 */
#include <outpost/coding/crc32.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using outpost::Crc32Reversed;

TEST(Crc32Test, initialValue)
{
    Crc32Reversed crc;

    EXPECT_EQ(0U, crc.getValue());
}

TEST(Crc32Test, randomTest1a)
{
    uint8_t data[] = "The quick brown fox jumps over the lazy dog";

    // Ignore the appended zero-byte
    size_t length = sizeof(data) - 1;

    EXPECT_EQ(0x414FA339U, Crc32Reversed::calculate(outpost::asSlice(data).first(length)));
}

TEST(Crc32Test, randomTest1b)
{
    uint8_t data[] = {0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x69, 0x63, 0x6B, 0x20, 0x62,
                      0x72, 0x6F, 0x77, 0x6E, 0x20, 0x66, 0x6F, 0x78, 0x20, 0x6A, 0x75,
                      0x6D, 0x70, 0x73, 0x20, 0x6F, 0x76, 0x65, 0x72, 0x20, 0x74, 0x68,
                      0x65, 0x20, 0x6C, 0x61, 0x7A, 0x79, 0x20, 0x64, 0x6F, 0x67};

    EXPECT_EQ(0x414FA339U, Crc32Reversed::calculate(outpost::asSlice(data)));
}

TEST(Crc32Test, randomTest2)
{
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    EXPECT_EQ(0x190A55ADU, Crc32Reversed::calculate(outpost::asSlice(data)));
}

TEST(Crc32Test, randomTest3)
{
    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    EXPECT_EQ(0xFF6CAB0BU, Crc32Reversed::calculate(outpost::asSlice(data)));
}

TEST(Crc32Test, randomTest4)
{
    uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
                      0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                      0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

    EXPECT_EQ(0x91267E8AU, Crc32Reversed::calculate(outpost::asSlice(data)));
}

TEST(Crc32Test, emptyTest)
{
    EXPECT_EQ(0x00000000U, Crc32Reversed::calculate(outpost::Slice<uint8_t>::empty()));
}

static uint32_t
crc32_update_bitwise_lsb_first(uint32_t crc, uint8_t data)
{
    uint8_t n = (crc ^ data) & 0xFF;

    uint32_t c = static_cast<uint32_t>(n);
    for (int k = 0; k < 8; k++)
    {
        if (c & 1)
        {
            c = 0xEDB88320L ^ (c >> 1);
        }
        else
        {
            c = c >> 1;
        }
    }

    crc = c ^ (crc >> 8);
    return crc;
}

TEST(Crc32Test, bitwiseTest)
{
    uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
                      0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                      0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

    uint32_t crc = 0xFFFFFFFF;
    for (auto d : data)
    {
        crc = crc32_update_bitwise_lsb_first(crc, d);
    }
    crc = crc ^ 0xFFFFFFFF;

    EXPECT_EQ(crc, Crc32Reversed::calculate(outpost::asSlice(data)));
}
