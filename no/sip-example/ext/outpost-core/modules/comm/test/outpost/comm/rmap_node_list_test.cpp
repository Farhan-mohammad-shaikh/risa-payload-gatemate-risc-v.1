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

#include <outpost/comm/rmap/rmap_node_list.h>

#include <gmock/gmock.h>

using namespace outpost;
using namespace outpost::comm;
using namespace ::testing;

TEST(RmapTargetNodeListTest, findNothing)
{
    RmapTargetsList targetNodeList{};

    EXPECT_EQ(targetNodeList.getTargetNode(1), nullptr);
    EXPECT_EQ(targetNodeList.getTargetNode("something"), nullptr);
}

TEST(RmapTargetNodeListTest, findAnElement)
{
    RmapTargetsList targetNodeList{};

    const auto id = 1;
    const auto targetAddress = 2;
    const auto key = 3;
    RmapTargetNode f{"first", id + 1, targetAddress + 1, key + 1};
    RmapTargetNode s{"second", id, targetAddress, key};

    ASSERT_TRUE(targetNodeList.addTargetNode(&f));
    ASSERT_TRUE(targetNodeList.addTargetNode(&s));

    EXPECT_EQ(targetNodeList.getTargetNode(targetAddress), &s);
    EXPECT_EQ(targetNodeList.getTargetNode("second"), &s);
}

TEST(RmapTargetNodeListTest, rejectsInvalidTargetNodeListConstructor)
{
    std::array<RmapTargetNode*, rmap::maxNumberOfTargetNodes + 1> targetNodes = {nullptr};
    RmapTargetsList targetNodeList{asSlice(targetNodes)};

    // constructor should not have succeeded
    EXPECT_THAT(targetNodeList.getSize(), ::testing::Eq(0));

    // we can add a target node
    EXPECT_TRUE(targetNodeList.addTargetNode(nullptr));
}

TEST(RmapTargetNodeListTest, allowsMaxSizeTargetNodeListConstructor)
{
    std::array<RmapTargetNode*, rmap::maxNumberOfTargetNodes> targetNodes = {nullptr};
    RmapTargetsList targetNodeList{asSlice(targetNodes)};

    // constructor should have succeeded
    EXPECT_THAT(targetNodeList.getSize(), ::testing::Eq(rmap::maxNumberOfTargetNodes));

    // so we can't add one additional target node
    EXPECT_FALSE(targetNodeList.addTargetNode(nullptr));
}

TEST(RmapTargetNodeListTest, allowsTargetNodeList)
{
    RmapTargetsList targetNodeList;

    std::array<RmapTargetNode*, rmap::maxNumberOfTargetNodes> targetNodes = {nullptr};
    EXPECT_TRUE(targetNodeList.addTargetNodes(asSlice(targetNodes)));
}

TEST(RmapTargetNodeListTest, rejectsInvalidTargetNodeList)
{
    RmapTargetsList targetNodeList;

    std::array<RmapTargetNode*, rmap::maxNumberOfTargetNodes + 1> targetNodes = {nullptr};
    // has not enough storage for addresses
    EXPECT_FALSE(targetNodeList.addTargetNodes(asSlice(targetNodes)));
}
