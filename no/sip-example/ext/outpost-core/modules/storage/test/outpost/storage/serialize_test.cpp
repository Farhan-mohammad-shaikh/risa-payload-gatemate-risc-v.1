/*
 * Copyright (c) 2014-2019, Fabian Greif
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/storage/serialize.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/harness.h>

using namespace outpost;

TEST(SerializeTest, store)
{
    uint8_t data[16] = {
            0,
    };

    EXPECT_EQ(0, data[0]);

    Serialize payload(data);

    payload.store<uint8_t>(0xab);

    EXPECT_EQ(0xab, data[0]);

    payload.store<uint16_t>(0xef12);

    EXPECT_EQ(0xef, data[1]);
    EXPECT_EQ(0x12, data[2]);

    payload.store24(0xa6c01a);

    EXPECT_EQ(0xa6, data[3]);
    EXPECT_EQ(0xc0, data[4]);
    EXPECT_EQ(0x1a, data[5]);

    payload.store<uint32_t>(0x61a9f51e);

    EXPECT_EQ(0x61, data[6]);
    EXPECT_EQ(0xa9, data[7]);
    EXPECT_EQ(0xf5, data[8]);
    EXPECT_EQ(0x1e, data[9]);
}

TEST(SerializeTest, shouldStorePackedData)
{
    uint8_t data[6] = {
            0,
    };

    Serialize payload(data);

    payload.storePacked12(0x123, 0x456);
    payload.storePacked12(0x789, 0xABC);

    EXPECT_EQ(0x12, data[0]);
    EXPECT_EQ(0x34, data[1]);
    EXPECT_EQ(0x56, data[2]);

    EXPECT_EQ(0x78, data[3]);
    EXPECT_EQ(0x9A, data[4]);
    EXPECT_EQ(0xBC, data[5]);
}

TEST(SerializeTest, storeTemplateExplicit)
{
    uint8_t data[20] = {
            0,
    };

    EXPECT_EQ(0, data[0]);

    Serialize payload(data);

    payload.store<uint8_t>(0xab);

    EXPECT_EQ(0xab, data[0]);

    payload.store<uint16_t>(0xef12);

    EXPECT_EQ(0xef, data[1]);
    EXPECT_EQ(0x12, data[2]);

    payload.store<uint32_t>(0x61a9f51e);

    EXPECT_EQ(0x61, data[3]);
    EXPECT_EQ(0xa9, data[4]);
    EXPECT_EQ(0xf5, data[5]);
    EXPECT_EQ(0x1e, data[6]);

    float f = 3.14159f;
    payload.store<float>(f);

    EXPECT_EQ(0x40, data[7]);
    EXPECT_EQ(0x49, data[8]);
    EXPECT_EQ(0x0F, data[9]);
    EXPECT_EQ(0xD0, data[10]);

    double d = 3.1415926535897931;
    payload.store<double>(d);

    EXPECT_EQ(0x40, data[11]);
    EXPECT_EQ(0x09, data[12]);
    EXPECT_EQ(0x21, data[13]);
    EXPECT_EQ(0xFB, data[14]);
    EXPECT_EQ(0x54, data[15]);
    EXPECT_EQ(0x44, data[16]);
    EXPECT_EQ(0x2D, data[17]);
    EXPECT_EQ(0x18, data[18]);

    EXPECT_EQ(19, payload.getPosition());
}

TEST(SerializeTest, storeTemplateImplicit)
{
    uint8_t data[20] = {
            0,
    };

    EXPECT_EQ(0, data[0]);

    Serialize payload(data);

    uint8_t u8 = 0xab;
    payload.store(u8);

    EXPECT_EQ(0xab, data[0]);
    EXPECT_EQ(1, payload.getPosition());

    uint16_t u16 = 0xef12;
    payload.store(u16);

    EXPECT_EQ(0xef, data[1]);
    EXPECT_EQ(0x12, data[2]);
    EXPECT_EQ(3, payload.getPosition());

    uint32_t u32 = 0x61a9f51e;
    payload.store(u32);

    EXPECT_EQ(0x61, data[3]);
    EXPECT_EQ(0xa9, data[4]);
    EXPECT_EQ(0xf5, data[5]);
    EXPECT_EQ(0x1e, data[6]);
    EXPECT_EQ(7, payload.getPosition());

    float f = 3.14159f;
    payload.store(f);

    EXPECT_EQ(0x40, data[7]);
    EXPECT_EQ(0x49, data[8]);
    EXPECT_EQ(0x0F, data[9]);
    EXPECT_EQ(0xD0, data[10]);
    EXPECT_EQ(11, payload.getPosition());

    double d = 3.1415926535897931;
    payload.store(d);

    EXPECT_EQ(0x40, data[11]);
    EXPECT_EQ(0x09, data[12]);
    EXPECT_EQ(0x21, data[13]);
    EXPECT_EQ(0xFB, data[14]);
    EXPECT_EQ(0x54, data[15]);
    EXPECT_EQ(0x44, data[16]);
    EXPECT_EQ(0x2D, data[17]);
    EXPECT_EQ(0x18, data[18]);

    EXPECT_EQ(19, payload.getPosition());
}

TEST(SerializeTest, storeFloat)
{
    float f = 3.14159f;

    uint8_t data[4];

    Serialize payload(data);

    payload.store<float>(f);

    EXPECT_EQ(0x40, data[0]);
    EXPECT_EQ(0x49, data[1]);
    EXPECT_EQ(0x0F, data[2]);
    EXPECT_EQ(0xD0, data[3]);

    EXPECT_EQ(4, payload.getPosition());
}

TEST(SerializeTest, storeDouble)
{
    double d = 3.1415926535897931;

    uint8_t data[8];

    Serialize payload(data);

    payload.store<double>(d);

    EXPECT_EQ(0x40, data[0]);
    EXPECT_EQ(0x09, data[1]);
    EXPECT_EQ(0x21, data[2]);
    EXPECT_EQ(0xFB, data[3]);
    EXPECT_EQ(0x54, data[4]);
    EXPECT_EQ(0x44, data[5]);
    EXPECT_EQ(0x2D, data[6]);
    EXPECT_EQ(0x18, data[7]);

    EXPECT_EQ(8, payload.getPosition());
}

TEST(SerializeTest, shouldRetrieveWrittenOutputAsSlice)
{
    uint8_t data[8];
    Serialize payload(data);

    auto slice = payload.asSlice();
    EXPECT_EQ(0U, slice.getNumberOfElements());
    EXPECT_OUTPOST_CONSTEXPR_ASSERTION_DEATH(slice[0], "Slice access out of bounds");

    payload.store(14.0f);

    slice = payload.asSlice();
    EXPECT_EQ(4U, slice.getNumberOfElements());
    EXPECT_EQ(&data[0], &slice[0]);
}
