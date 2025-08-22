/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2020-2021, Jan-Gerd Mess
 * Copyright (c) 2022, Adrian Roeser
 * Copyright (c) 2022, Pfeffer, Tobias
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/utils/log2.h>

#include <unittest/harness.h>

#include <stdint.h>

using namespace outpost;

TEST(Log2, test)
{
    EXPECT_EQ(0U, outpost::Log2(0));
    EXPECT_EQ(0U, outpost::Log2(1));
    EXPECT_EQ(1U, outpost::Log2(2));
    EXPECT_EQ(4U, outpost::Log2(16));
    EXPECT_EQ(10U, outpost::Log2(1024));
    EXPECT_EQ(12U, outpost::Log2(4096));
    EXPECT_EQ(63U, outpost::Log2(std::numeric_limits<size_t>::max()));
}
