/*
 * Copyright (c) 2019, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/utils/minmax.h>

#include <unittest/harness.h>

#include <stdint.h>

using namespace outpost::utils;

TEST(maxTest, test)
{
    EXPECT_EQ(0xffu, max<uint8_t>(0, 0xff));
    EXPECT_EQ(0xffu, max<uint8_t>(0xff, 0));
    EXPECT_EQ(0xffu, max<uint8_t>(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0xff));
    EXPECT_EQ(0xffu, max<uint8_t>(0xff, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0));
    EXPECT_EQ(201u,
              max<uint8_t>(9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 11, 26, 25, 83, 47, 49, 30, 57, 111, 201));
}

TEST(maxTest, differentTypes)
{
    EXPECT_EQ(INT32_MAX,
              max<int32_t>(
                      'c', -182, 38883, (INT64_MAX - 100l), 111245224, INT32_MAX, -472, 0, 292u));
}

TEST(maxTest, castBeforeCompare)
{
    EXPECT_EQ(0xffu, max<uint8_t>(0x100, 0xff, 0x82, 0x1fe, 3));
}

TEST(minTest, test)
{
    EXPECT_EQ(0, min<uint8_t>(0, 0xff));
    EXPECT_EQ(0, min<uint8_t>(0xff, 0));
    EXPECT_EQ(0, min<uint8_t>(12, 1, 2, 3, 4, 5, 6, 0, 8, 9, 10, 11, 0xff));
    EXPECT_EQ(0, min<uint8_t>(0xff, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0));
    EXPECT_EQ(0,
              min<uint8_t>(9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 11, 26, 25, 83, 47, 49, 30, 57, 111, 201));
}

TEST(minTest, differentTypes)
{
    EXPECT_EQ(INT32_MIN,
              min<int32_t>(
                      'c', -182, 38883, (INT64_MIN + 100l), 111245224, INT32_MIN, -472, 0u, 292));
}

TEST(minTest, castBeforeCompare)
{
    EXPECT_EQ(3, min<uint8_t>(0x108, 0x2ff, 0x82, 0x1fe, 0xff03, 5));
}
