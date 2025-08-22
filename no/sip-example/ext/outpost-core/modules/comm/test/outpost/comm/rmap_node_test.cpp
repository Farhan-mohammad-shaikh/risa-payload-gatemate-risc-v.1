/*
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/comm/rmap/rmap_node.h>

#include <gmock/gmock.h>

using namespace outpost;
using namespace outpost::comm;
using namespace ::testing;

TEST(RmapTargetNodeTest, instantiateWithoutParameter)
{
    RmapTargetNode dut;

    EXPECT_EQ(dut.getId(), 0);
    EXPECT_EQ(dut.getTargetLogicalAddress(), rmap::defaultLogicalAddress);
    EXPECT_EQ(dut.getKey(), 0);

    const auto expectedName = "Default";
    const auto actualName = dut.getName();
    ASSERT_NE(actualName, nullptr);
    EXPECT_EQ(strcmp(expectedName, actualName), 0);
}

TEST(RmapTargetNodeTest, instantiateWithTooBigName)
{
    const auto tooLongName = "REEEEEEAAAAALLLLYYYY_LOOONNNNGGGGGG";
    ASSERT_GT(strlen(tooLongName), rmap::maxNodeNameLength) << "Testing setup is not enough";

    RmapTargetNode dut{tooLongName, 1, 2, 3};

    EXPECT_EQ(dut.getId(), 1);
    EXPECT_EQ(dut.getTargetLogicalAddress(), 2);
    EXPECT_EQ(dut.getKey(), 3);

    const auto expectedName = "Default";
    const auto actualName = dut.getName();
    ASSERT_NE(actualName, nullptr);
    EXPECT_EQ(strcmp(expectedName, actualName), 0);
}
