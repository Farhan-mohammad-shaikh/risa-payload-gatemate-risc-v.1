/*
 * Copyright (c) 2013-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/utils/pow.h>

#include <unittest/harness.h>

#include <stdint.h>

using namespace outpost;

TEST(PowerOfTwoTest, test)
{
    EXPECT_EQ(8, outpost::PowerOfTwo<3>::value);
    EXPECT_EQ(256 * 256 * 256, outpost::PowerOfTwo<24>::value);
}

TEST(PowTest, test)
{
    int32_t value;

    value = outpost::Pow<2, 3>::value;
    EXPECT_EQ(8, value);

    value = outpost::Pow<2, 0>::value;
    EXPECT_EQ(1, value);

    value = outpost::Pow<2, -1>::value;
    EXPECT_EQ(0, value);

    value = outpost::Pow<3, 4>::value;
    EXPECT_EQ(81, value);
}
