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

#include <outpost/storage/bitorder.h>

#include <unittest/harness.h>

using outpost::BitorderMsb0ToLsb0;

TEST(BitorderTest, convert16Bit)
{
    typedef BitorderMsb0ToLsb0<uint16_t, 0, 15> Converter;

    EXPECT_EQ(0U, static_cast<size_t>(Converter::byteIndex));
    EXPECT_EQ(16U, static_cast<size_t>(Converter::width));
    EXPECT_EQ(15U, static_cast<size_t>(Converter::start));
    EXPECT_EQ(0U, static_cast<size_t>(Converter::end));
}

TEST(BitorderTest, convertWithOffset)
{
    typedef BitorderMsb0ToLsb0<uint16_t, 13, 21> Converter;

    EXPECT_EQ(1U, static_cast<size_t>(Converter::byteIndex));
    EXPECT_EQ(9U, static_cast<size_t>(Converter::width));
    EXPECT_EQ(10U, static_cast<size_t>(Converter::start));
    EXPECT_EQ(2U, static_cast<size_t>(Converter::end));
}

TEST(BitorderTest, convertWithBigOffset)
{
    typedef BitorderMsb0ToLsb0<uint16_t, 150, 154> Converter;

    EXPECT_EQ(18U, static_cast<size_t>(Converter::byteIndex));
    EXPECT_EQ(5U, static_cast<size_t>(Converter::width));
    EXPECT_EQ(9U, static_cast<size_t>(Converter::start));
    EXPECT_EQ(5U, static_cast<size_t>(Converter::end));
}

TEST(BitorderTest, convertWithOffset32Bit)
{
    typedef BitorderMsb0ToLsb0<uint32_t, 13, 21> Converter;

    EXPECT_EQ(1U, static_cast<size_t>(Converter::byteIndex));
    EXPECT_EQ(9U, static_cast<size_t>(Converter::width));
    EXPECT_EQ(26U, static_cast<size_t>(Converter::start));
    EXPECT_EQ(18U, static_cast<size_t>(Converter::end));
}
