/*
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/parameter/type.h>
#include <outpost/time.h>

#include <unittest/harness.h>

#include <stdint.h>

using namespace outpost::parameter;

TEST(TypeTest, comparing)
{
    EXPECT_EQ(Type::getType<int>(), Type::getType<int>());
    int i = 0;
    float f = 0.0f;
    // base comparison
    EXPECT_EQ(Type::getType(i), Type::getType(i));
    EXPECT_EQ(Type::getType<int>(), Type::getType<int>());
    EXPECT_NE(Type::getType<float>(), Type::getType<int>());
    EXPECT_NE(Type::getType(i), Type::getType(f));

    const int i2 = 10;
    int& i3 = i;
    const int& i4 = i2;
    // const ref does not matter
    EXPECT_EQ(Type::getType(i), Type::getType<int>());
    EXPECT_EQ(Type::getType(i2), Type::getType<int>());
    EXPECT_EQ(Type::getType(i3), Type::getType<int>());
    EXPECT_EQ(Type::getType(i4), Type::getType<int>());

    EXPECT_EQ(Type::getType(i), Type::getType<const int>());
    EXPECT_EQ(Type::getType(i2), Type::getType<const int>());
    EXPECT_EQ(Type::getType(i3), Type::getType<const int>());
    EXPECT_EQ(Type::getType(i4), Type::getType<const int>());

    EXPECT_EQ(Type::getType(i), Type::getType<int&>());
    EXPECT_EQ(Type::getType(i2), Type::getType<int&>());
    EXPECT_EQ(Type::getType(i3), Type::getType<int&>());
    EXPECT_EQ(Type::getType(i4), Type::getType<int&>());

    EXPECT_EQ(Type::getType(i), Type::getType<const int&>());
    EXPECT_EQ(Type::getType(i2), Type::getType<const int&>());
    EXPECT_EQ(Type::getType(i3), Type::getType<const int&>());
    EXPECT_EQ(Type::getType(i4), Type::getType<const int&>());
}
