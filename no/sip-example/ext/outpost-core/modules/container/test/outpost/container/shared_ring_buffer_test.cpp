/*
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/container/shared_buffer.h>
#include <outpost/container/shared_object_pool.h>
#include <outpost/container/shared_ring_buffer.h>
#include <outpost/rtos/thread.h>
#include <outpost/storage/serialize.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace outpost;
using namespace container;

static constexpr size_t poolSize = 15;
static constexpr size_t slots = 10;
static constexpr size_t objectSize = 160;

class SharedRingBufferTest : public testing::Test
{
public:
    SharedBufferPool<objectSize, poolSize> mPool;
    SharedRingBufferStorage<slots> mRing;

    SharedRingBufferTest()
    {
    }

    virtual void
    SetUp()
    {
    }

    virtual void
    TearDown()
    {
    }
};

TEST_F(SharedRingBufferTest, peekEmpty)
{
    SharedBufferPointer p;
    p = mRing.peek();
    EXPECT_FALSE(p.isValid());

    p = mRing.peek(4096);
    EXPECT_FALSE(p.isValid());
}

TEST_F(SharedRingBufferTest, resetRingbuffer)
{
    mRing.reset();
    EXPECT_TRUE(mRing.isEmpty());
    EXPECT_EQ(mRing.getFreeSlots(), slots);
    EXPECT_EQ(mRing.getUsedSlots(), 0u);

    SharedBufferPointer p;
    ASSERT_TRUE(mPool.allocate(p));
    mRing.append(p);

    EXPECT_FALSE(mRing.isEmpty());
    EXPECT_EQ(mRing.getFreeSlots(), slots - 1);
    EXPECT_EQ(mRing.getUsedSlots(), 1u);

    mRing.reset();
    EXPECT_TRUE(mRing.isEmpty());
    EXPECT_EQ(mRing.getFreeSlots(), slots);
    EXPECT_EQ(mRing.getUsedSlots(), 0u);
}

TEST_F(SharedRingBufferTest, queueBuffer)
{
    EXPECT_TRUE(mRing.isEmpty());
    EXPECT_EQ(mRing.getFreeSlots(), slots);
    EXPECT_EQ(mRing.getUsedSlots(), 0u);

    {  // scoping lifetime of shared buffers
        SharedBufferPointer p1;
        ASSERT_TRUE(mPool.allocate(p1));
        EXPECT_TRUE(p1.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            p1[i] = 0x01;
        }
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
        EXPECT_TRUE(mRing.append(p1));
        EXPECT_FALSE(mRing.isEmpty());
        EXPECT_EQ(mRing.getFreeSlots(), slots - 1);
        EXPECT_EQ(mRing.getUsedSlots(), 1u);
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    {
        SharedBufferPointer p1;
        ASSERT_TRUE(mPool.allocate(p1));
        EXPECT_TRUE(p1.isValid());

        for (size_t i = 0; i < objectSize; i++)
        {
            p1[i] = 0x02;
        }

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);
        EXPECT_TRUE(mRing.append(p1, 0x11));
        EXPECT_FALSE(mRing.isEmpty());
        EXPECT_EQ(mRing.getFreeSlots(), slots - 2);
        EXPECT_EQ(mRing.getUsedSlots(), 2u);
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);

    {
        SharedBufferPointer p2;
        // checking peek
        p2 = mRing.peek();
        EXPECT_TRUE(p2.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            EXPECT_EQ(p2[i], 0x01);
        }
        // checking peek with offset
        p2 = mRing.peek(1);
        EXPECT_TRUE(p2.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            EXPECT_EQ(p2[i], 0x02);
        }
        EXPECT_EQ(mRing.peekFlags(), 0x00);
        EXPECT_EQ(mRing.peekFlags(mRing.getUsedSlots() - 1), 0x11);
        EXPECT_TRUE(mRing.setFlags(0xef, mRing.getUsedSlots() - 1));

        // check removal of the first element
        p2 = mRing.peek();
        EXPECT_TRUE(mRing.pop());
        EXPECT_EQ(mRing.peekFlags(), 0xef);
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);
        EXPECT_FALSE(mRing.isEmpty());
    }
    // p2 has been freed now
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);

    {
        SharedBufferPointer p2;
        p2 = mRing.peek(0);

        ASSERT_TRUE(p2.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            EXPECT_EQ(p2[i], 0x02);
        }
        EXPECT_EQ(mRing.peekFlags(), 0xef);
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
        EXPECT_TRUE(mRing.pop());
        EXPECT_TRUE(mRing.isEmpty());
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);

    {  // nothing to get from the queue
        SharedBufferPointer p2;
        p2 = mRing.peek();

        ASSERT_TRUE(!p2.isValid());
        EXPECT_EQ(mRing.peekFlags(), 0x00);
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
        EXPECT_FALSE(mRing.pop());
        EXPECT_TRUE(mRing.isEmpty());
    }
}

TEST_F(SharedRingBufferTest, indexOutOfBounds)
{
    EXPECT_EQ(mRing.peekFlags(0), 0U);
    EXPECT_EQ(mRing.peekFlags(mRing.getFreeSlots() - 1), 0U);

    EXPECT_FALSE(mRing.setFlags(0xff, 0));
    EXPECT_EQ(mRing.peekFlags(0), 0U);
    EXPECT_FALSE(mRing.setFlags(0xff, mRing.getFreeSlots()));
    EXPECT_EQ(mRing.peekFlags(mRing.getFreeSlots()), 0U);

    EXPECT_EQ(mRing.peek(), SharedBufferPointer());
    EXPECT_EQ(mRing.peek(mRing.getFreeSlots() - 1), SharedBufferPointer());
    EXPECT_EQ(mRing.peek(mRing.getFreeSlots()), SharedBufferPointer());
}
