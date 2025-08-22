/*
 * Copyright (c) 2018-2021, 2023, Jan-Gerd Mess
 * Copyright (c) 2018, Fabian Greif
 * Copyright (c) 2019, 2021, Jan Malburg
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Pfeffer, Tobias
 * Copyright (c) 2022, Passenberg, Felix Constantin
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/container/reference_queue.h>
#include <outpost/container/shared_buffer.h>
#include <outpost/container/shared_object_pool.h>
#include <outpost/rtos/thread.h>
#include <outpost/storage/serialize.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

using namespace testing;
using namespace outpost;
using namespace outpost::container;
using namespace outpost::utils;

static constexpr size_t poolSize = 1500;
static constexpr size_t objectSize = 160;
static std::array<uint8_t, poolSize * objectSize> DataBuffer;

class ExternalSharedBufferTest : public testing::Test
{
public:
    ExternalSharedBufferPool<objectSize, poolSize> mPool{DataBuffer.data()};
    ReferenceQueue<SharedBufferPointer, 10> mQueue;

    void
    passByRef(SharedBufferPointer& p);

    void
    passByValue(SharedBufferPointer p);
};

TEST_F(ExternalSharedBufferTest, isInitialized)
{
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
    EXPECT_EQ(mPool.numberOfElements(), poolSize);
    EXPECT_EQ(mPool.getElementSize(), objectSize);
}

TEST_F(ExternalSharedBufferTest, allocateBuffer)
{
    SharedBufferPointer p1;
    SharedBufferPointer p2;
    EXPECT_TRUE(mPool.allocate(p1));
    EXPECT_TRUE(p1.isValid());
    EXPECT_EQ(p1->getReferenceCount(), 1U);
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    EXPECT_EQ(mPool.numberOfElements(), poolSize);

    EXPECT_TRUE(mPool.allocate(p2));
    EXPECT_TRUE(p2.isValid());
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);
    EXPECT_TRUE(p2 != p1);
}

TEST_F(ExternalSharedBufferTest, constructorTest)
{
    SharedBufferPointer p1;
    EXPECT_EQ(p1.asSlice().getNumberOfElements(), 0U);

    Slice<uint8_t> slice = static_cast<Slice<uint8_t>>(p1);
    EXPECT_EQ(slice.getNumberOfElements(), 0U);

    Slice<const uint8_t> constSlice = static_cast<Slice<const uint8_t>>(p1);
    EXPECT_EQ(constSlice.getNumberOfElements(), 0U);

    const uint8_t* ptr = static_cast<uint8_t*>(p1);
    EXPECT_EQ(ptr, nullptr);

    EXPECT_TRUE(mPool.allocate(p1));
    EXPECT_EQ(p1->getReferenceCount(), 1U);
    SharedBufferPointer p2;
    EXPECT_EQ(p1->getReferenceCount(), 1U);
    p2 = p1;
    EXPECT_EQ(p1->getReferenceCount(), 2U);

    SharedBufferPointer p3(p1);
    EXPECT_EQ(p1->getReferenceCount(), 3U);

    {
        SharedBufferPointer p4 = SharedBufferPointer(p3);
        EXPECT_EQ(p1->getReferenceCount(), 4U);
        SharedChildPointer ch2;
        {
            SharedChildPointer ch1;
            EXPECT_EQ(p1->getReferenceCount(), 4U);
            p1.getChild(ch1, 0, 0, 1);
            EXPECT_EQ(p1->getReferenceCount(), 6U);

            {
                EXPECT_EQ(p1->getReferenceCount(), 6U);
                ch1.getChild(ch2, 0, 0, 1);
                EXPECT_EQ(p1->getReferenceCount(), 8U);
            }
        }
        EXPECT_EQ(p1->getReferenceCount(), 6U);

        SharedChildPointer ch3 = SharedChildPointer(ch2);
    }

    EXPECT_EQ(p1->getReferenceCount(), 3U);
}

TEST_F(ExternalSharedBufferTest, constConstructorTest)
{
    ConstSharedBufferPointer p1;

    EXPECT_EQ(p1.asSlice().getNumberOfElements(), 0U);

    Slice<const uint8_t> slice = static_cast<Slice<const uint8_t>>(p1);
    EXPECT_EQ(slice.getNumberOfElements(), 0U);

    const uint8_t* ptr = static_cast<const uint8_t*>(p1);
    EXPECT_EQ(ptr, nullptr);

    EXPECT_TRUE(mPool.allocate(p1));
    EXPECT_EQ(p1->getReferenceCount(), 1U);
    ConstSharedBufferPointer p2;
    EXPECT_EQ(p1->getReferenceCount(), 1U);
    p2 = p1;
    EXPECT_EQ(p1->getReferenceCount(), 2U);

    ConstSharedBufferPointer p3(p1);
    EXPECT_EQ(p1->getReferenceCount(), 3U);

    {
        ConstSharedBufferPointer p4 = ConstSharedBufferPointer(p3);
        EXPECT_EQ(p1->getReferenceCount(), 4U);
        ConstSharedChildPointer ch2;
        {
            ConstSharedChildPointer ch1;
            EXPECT_EQ(p1->getReferenceCount(), 4U);
            p1.getChild(ch1, 0, 0, 1);
            EXPECT_EQ(p1->getReferenceCount(), 6U);

            {
                EXPECT_EQ(p1->getReferenceCount(), 6U);
                ch1.getChild(ch2, 0, 0, 1);
                EXPECT_EQ(p1->getReferenceCount(), 8U);
            }
        }
        EXPECT_EQ(p1->getReferenceCount(), 6U);

        ConstSharedChildPointer ch3 = ConstSharedChildPointer(ch2);
    }

    EXPECT_EQ(p1->getReferenceCount(), 3U);
}

TEST_F(ExternalSharedBufferTest, deleteParentFirst)
{
    {
        SharedChildPointer ch1;
        {
            SharedBufferPointer p1;
            mPool.allocate(p1);
            EXPECT_EQ(p1->getReferenceCount(), 1U);
            p1.getChild(ch1, 0, 0, 1);
            EXPECT_EQ(p1->getReferenceCount(), 3U);
            EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
            passByRef(p1);
            EXPECT_EQ(p1->getReferenceCount(), 3U);
            EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
            passByValue(p1);
            EXPECT_EQ(p1->getReferenceCount(), 3U);
            EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
        }
        EXPECT_EQ(ch1->getReferenceCount(), 2U);
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    }
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
}

void
ExternalSharedBufferTest::passByRef(SharedBufferPointer& p)
{
    EXPECT_EQ(p->getReferenceCount(), 3U);

    {
        SharedBufferPointer p_temp = p;
        EXPECT_EQ(p->getReferenceCount(), 4U);
    }

    EXPECT_EQ(p->getReferenceCount(), 3U);
}

void
ExternalSharedBufferTest::passByValue(SharedBufferPointer p)
{
    EXPECT_EQ(p->getReferenceCount(), 4U);

    {
        SharedBufferPointer p_temp = p;
        EXPECT_EQ(p->getReferenceCount(), 5U);
    }

    EXPECT_EQ(p->getReferenceCount(), 4U);
}

TEST_F(ExternalSharedBufferTest, deallocateBuffer)
{
    {
        SharedBufferPointer p1;
        EXPECT_TRUE(mPool.allocate(p1));
        EXPECT_TRUE(p1.isValid());
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
}

TEST_F(ExternalSharedBufferTest, allocateFullPool)
{
    SharedBufferPointer p[poolSize];
    for (size_t i = 0; i < poolSize; i++)
    {
        EXPECT_TRUE(mPool.allocate(p[i]));
        EXPECT_TRUE(p[i].isValid());
    }

    SharedBufferPointer p_false;
    EXPECT_FALSE(mPool.allocate(p_false));
    EXPECT_FALSE(p_false.isValid());
}

TEST_F(ExternalSharedBufferTest, queueBuffer)
{
    ReferenceQueue<SharedBufferPointer, 2> q;
    EXPECT_TRUE(q.isEmpty());
    EXPECT_FALSE(q.isFull());

    {
        SharedBufferPointer p1;
        ASSERT_TRUE(mPool.allocate(p1));
        EXPECT_TRUE(p1.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            p1[i] = 0x01;
        }
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
        EXPECT_EQ(q.send(p1), OperationResult::success);
        EXPECT_FALSE(q.isEmpty());
        EXPECT_FALSE(q.isFull());
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
        EXPECT_EQ(q.send(p1), OperationResult::success);
        EXPECT_FALSE(q.isEmpty());
        EXPECT_TRUE(q.isFull());
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);

    {
        SharedBufferPointer p2;
        auto res = q.receive(time::Duration::zero());
        ASSERT_TRUE(res);
        p2 = *res;
        EXPECT_TRUE(p2.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            EXPECT_EQ(p2[i], 0x01);
        }
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);
        EXPECT_FALSE(q.isEmpty());
        EXPECT_FALSE(q.isFull());
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);

    {
        SharedBufferPointer p2;
        auto res = q.receive(time::Duration::zero());
        ASSERT_TRUE(res);
        p2 = *res;

        ASSERT_TRUE(p2.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            EXPECT_EQ(p2[i], 0x02);
        }
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
        EXPECT_TRUE(q.isEmpty());
        EXPECT_FALSE(q.isFull());
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
}

TEST_F(ExternalSharedBufferTest, queueConstBuffer)
{
    ReferenceQueue<ConstSharedBufferPointer, 2> q;
    EXPECT_TRUE(q.isEmpty());
    EXPECT_FALSE(q.isFull());

    {
        SharedBufferPointer p1;
        ASSERT_TRUE(mPool.allocate(p1));
        EXPECT_TRUE(p1.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            p1[i] = 0x03;
        }
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
        ConstSharedBufferPointer constP1 = p1;
        EXPECT_EQ(q.send(constP1), OperationResult::success);
        EXPECT_FALSE(q.isEmpty());
        EXPECT_FALSE(q.isFull());
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    {
        SharedBufferPointer p1;
        ASSERT_TRUE(mPool.allocate(p1));
        EXPECT_TRUE(p1.isValid());

        for (size_t i = 0; i < objectSize; i++)
        {
            p1[i] = 0x04;
        }

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);
        ConstSharedBufferPointer constP1 = p1;
        EXPECT_EQ(q.send(constP1), OperationResult::success);
        EXPECT_FALSE(q.isEmpty());
        EXPECT_TRUE(q.isFull());
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);

    {
        ConstSharedBufferPointer p2;
        auto res = q.receive(time::Duration::zero());
        ASSERT_TRUE(res);
        p2 = *res;

        EXPECT_TRUE(p2.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            EXPECT_EQ(p2[i], 0x03);
        }
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);
        EXPECT_FALSE(q.isEmpty());
        EXPECT_FALSE(q.isFull());
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);

    {
        ConstSharedBufferPointer p2;
        auto res = q.receive(time::Duration::zero());
        ASSERT_TRUE(res);
        p2 = *res;

        ASSERT_TRUE(p2.isValid());
        for (size_t i = 0; i < objectSize; i++)
        {
            EXPECT_EQ(p2[i], 0x04);
        }
        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
        EXPECT_TRUE(q.isEmpty());
        EXPECT_FALSE(q.isFull());
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
}

TEST_F(ExternalSharedBufferTest, allocateChildBuffer)
{
    {
        SharedChildPointer child;

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);

        {
            SharedBufferPointer mPointer;
            EXPECT_TRUE(mPool.allocate(mPointer));
            EXPECT_TRUE(mPointer.isValid());
            for (size_t i = 0; i < objectSize; i++)
            {
                mPointer[i] = i;
            }

            EXPECT_TRUE(mPointer.getChild(child, 1, 5, 5));
            EXPECT_TRUE(child.isValid());

            EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
        }

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);

        {
            SharedBufferPointer mPointer = child.getParent();

            SharedChildPointer newChild;

            EXPECT_TRUE(mPointer.getChild(newChild, 3, 2, 3));
            EXPECT_TRUE(newChild.isValid());

            Slice<const uint8_t> mArray(static_cast<Slice<const uint8_t>>(newChild));
            EXPECT_EQ(mArray.getNumberOfElements(), newChild.getLength());
            for (size_t i = 0; i < newChild.getLength(); i++)
            {
                EXPECT_EQ(mArray[i], i + 2);
            }
        }

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
}

TEST_F(ExternalSharedBufferTest, twoElementPool)
{
    SharedBufferPool<1, 2> pool;
    SharedBufferPointer p1, p2;
    EXPECT_TRUE(pool.allocate(p1));
    EXPECT_TRUE(pool.allocate(p2));
}

TEST_F(ExternalSharedBufferTest, threeElementPool)
{
    SharedBufferPool<1, 3> pool;
    SharedBufferPointer p1, p2, p3;
    EXPECT_TRUE(pool.allocate(p1));
    EXPECT_TRUE(pool.allocate(p2));
    EXPECT_TRUE(pool.allocate(p3));
}

TEST_F(ExternalSharedBufferTest, allocateChildChildBuffer)
{
    {
        SharedChildPointer child;
        SharedChildPointer childChild;

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);

        {
            SharedBufferPointer mPointer;
            EXPECT_TRUE(mPool.allocate(mPointer));
            EXPECT_TRUE(mPointer.isValid());
            for (size_t i = 0; i < objectSize; i++)
            {
                mPointer[i] = i + 1;  // other value then previous tests
            }

            EXPECT_TRUE(mPointer.getChild(child, 1, 5, 5));
            EXPECT_TRUE(child.isValid());

            EXPECT_TRUE(child.getChild(childChild, 2, 0, 5));
            EXPECT_TRUE(childChild.isValid());
            EXPECT_TRUE(childChild.getOrigin() == mPointer);
            EXPECT_TRUE(childChild.getParent() == child);
            EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);

            SharedChildPointer childChildChild;
            EXPECT_TRUE(childChild.getChild(childChildChild, 3, 0, 5));
            EXPECT_TRUE(childChildChild.getOrigin() == mPointer);
            EXPECT_TRUE(childChildChild == mPointer);
        }

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);

        {
            SharedBufferPointer mPointer = child.getParent();

            SharedChildPointer newChild;

            EXPECT_TRUE(mPointer.getChild(newChild, 3, 2, 3));
            EXPECT_TRUE(newChild.isValid());

            EXPECT_TRUE(newChild.isChild());

            EXPECT_FALSE(mPointer.isChild());

            Slice<const uint8_t> mArray(static_cast<Slice<const uint8_t>>(newChild));
            EXPECT_EQ(mArray.getNumberOfElements(), newChild.getLength());
            for (size_t i = 0; i < newChild.getLength(); i++)
            {
                EXPECT_EQ(mArray[i], i + 1 + 2);
            }
        }

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
}

TEST_F(ExternalSharedBufferTest, allocateConstChildChildBuffer)
{
    {
        ConstSharedChildPointer child;
        ConstSharedChildPointer childChild;

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);

        {
            SharedBufferPointer mPointerWriteable;
            EXPECT_TRUE(mPool.allocate(mPointerWriteable));
            EXPECT_TRUE(mPointerWriteable.isValid());
            for (size_t i = 0; i < objectSize; i++)
            {
                mPointerWriteable[i] = i + 2;  // other value then the previous tests
            }

            ConstSharedBufferPointer mPointer = mPointerWriteable;
            mPointerWriteable = SharedBufferPointer();
            EXPECT_TRUE(mPointer.isValid());

            EXPECT_TRUE(mPointer.getChild(child, 1, 5, 5));
            EXPECT_TRUE(child.isValid());

            EXPECT_TRUE(child.getChild(childChild, 2, 0, 5));
            EXPECT_TRUE(childChild.isValid());
            EXPECT_TRUE(childChild.getOrigin() == mPointer);
            EXPECT_TRUE(childChild.getParent() == child);
            EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);

            ConstSharedChildPointer childChildChild;
            EXPECT_TRUE(childChild.getChild(childChildChild, 3, 0, 5));
            EXPECT_TRUE(childChildChild.getOrigin() == mPointer);
            EXPECT_TRUE(childChildChild == mPointer);
        }

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);

        {
            ConstSharedBufferPointer mPointer = child.getParent();
            ConstSharedChildPointer newChild;

            EXPECT_TRUE(mPointer.getChild(newChild, 3, 2, 3));
            EXPECT_TRUE(newChild.isValid());
            EXPECT_TRUE(newChild.isChild());
            EXPECT_FALSE(mPointer.isChild());

            Slice<const uint8_t> mArray(static_cast<Slice<const uint8_t>>(newChild));
            EXPECT_EQ(mArray.getNumberOfElements(), newChild.getLength());
            for (size_t i = 0; i < newChild.getLength(); i++)
            {
                EXPECT_EQ(mArray[i], i + 2 + 2);
            }
        }

        EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    }

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
}

TEST_F(ExternalSharedBufferTest, constPointerKeepsAllocated)
{
    ConstSharedBufferPointer c1;
    SharedBufferPointer p1;

    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);

    EXPECT_TRUE(mPool.allocate(c1));
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    EXPECT_TRUE(mPool.allocate(p1));
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 2);
    c1 = p1;
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    p1 = SharedBufferPointer();
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize - 1);
    c1 = ConstSharedBufferPointer();  // cppcheck-suppress redundantAssignment
    EXPECT_EQ(mPool.numberOfFreeElements(), poolSize);
}
