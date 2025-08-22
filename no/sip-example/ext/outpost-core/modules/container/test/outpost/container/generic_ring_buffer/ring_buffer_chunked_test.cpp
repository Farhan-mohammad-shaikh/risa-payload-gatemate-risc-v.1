/*
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Cold, Erin Cynthia
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

/**
 * \file
 * \brief   Test RingBuffer functions
 *
 * \author  Fabian Greif
 */

#include <outpost/container/ring_buffer_allocators/array_ring_buffer_chunked.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/harness.h>

using namespace outpost;
using namespace container;

class ByteArrayRingBufferTest : public testing::Test
{
public:
    static const size_t maximumChunkLength = 100;  // over 64 for testing zero append
    static const size_t numberOfChunks = 10;

    void
    fillWithTestData(size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            defaultData[0] = i;
            defaultData[1] = i + 10;
            EXPECT_TRUE(buffer.append(asSlice(defaultData).first(2)));
        }
    }

    ChunkedRingBufferArrayStorage<maximumChunkLength, numberOfChunks> buffer;

    uint8_t defaultData[maximumChunkLength];
};

const size_t ByteArrayRingBufferTest::maximumChunkLength;
const size_t ByteArrayRingBufferTest::numberOfChunks;

TEST_F(ByteArrayRingBufferTest, initialValues)
{
    EXPECT_EQ(numberOfChunks, buffer.getFreeSlots());
    EXPECT_EQ(maximumChunkLength, buffer.getChunkSize());
    EXPECT_EQ(0U, buffer.getUsedSlots());
    EXPECT_TRUE(buffer.isEmpty());
}

TEST_F(ByteArrayRingBufferTest, rejectOverlongData)
{
    uint8_t data[maximumChunkLength + 1];
    EXPECT_FALSE(buffer.append(asSlice(data)));
}

TEST_F(ByteArrayRingBufferTest, acceptShorterData)
{
    EXPECT_TRUE(buffer.append(Slice<uint8_t>::empty()));
    EXPECT_TRUE(buffer.append(asSlice(defaultData).first(1)));
    EXPECT_TRUE(buffer.append(asSlice(defaultData).first(maximumChunkLength - 1)));
    EXPECT_TRUE(buffer.append(asSlice(defaultData).first(maximumChunkLength)));
}

TEST_F(ByteArrayRingBufferTest, writeIncreasesElementCount)
{
    EXPECT_TRUE(buffer.append(asSlice(defaultData)));
    EXPECT_TRUE(buffer.append(asSlice(defaultData)));
    EXPECT_EQ(2U, buffer.getUsedSlots());
}

TEST_F(ByteArrayRingBufferTest, appendIsRejectedAfterBufferIsFull)
{
    for (size_t i = 0; i < numberOfChunks; ++i)
    {
        EXPECT_TRUE(buffer.append(asSlice(defaultData)));
    }
    EXPECT_EQ(numberOfChunks, buffer.getUsedSlots());

    EXPECT_FALSE(buffer.append(asSlice(defaultData)));
    EXPECT_EQ(numberOfChunks, buffer.getUsedSlots());
}

TEST_F(ByteArrayRingBufferTest, retrieveAndVerifyElements)
{
    fillWithTestData(5);

    for (size_t i = 0; i < 5; ++i)
    {
        ASSERT_FALSE(buffer.isEmpty());
        Slice<const uint8_t> chunk = buffer.peek();
        EXPECT_EQ(2U, chunk.getNumberOfElements());

        EXPECT_EQ(i, chunk[0]);
        EXPECT_EQ(i + 10, chunk[1]);
        EXPECT_TRUE(buffer.pop());
    }
    EXPECT_TRUE(buffer.isEmpty());
}

TEST_F(ByteArrayRingBufferTest, peek)
{
    fillWithTestData(5);

    for (size_t i = 0; i < 5; ++i)
    {
        Slice<const uint8_t> chunk = buffer.peek(i);
        EXPECT_EQ(2U, chunk.getNumberOfElements());

        EXPECT_EQ(i, chunk[0]);
        EXPECT_EQ(i + 10, chunk[1]);
    }
}

TEST_F(ByteArrayRingBufferTest, invalidPeekValueReturnsZero)
{
    EXPECT_EQ(0U, buffer.peek(100000).getNumberOfElements());
}

TEST_F(ByteArrayRingBufferTest, resetDropsAllContent)
{
    fillWithTestData(5);

    buffer.reset();

    EXPECT_EQ(numberOfChunks, buffer.getFreeSlots());
    EXPECT_EQ(0U, buffer.getUsedSlots());
    EXPECT_TRUE(buffer.isEmpty());
    EXPECT_EQ(0U, buffer.peek(1).getNumberOfElements());
}

TEST_F(ByteArrayRingBufferTest, resetAfterDropsSomeContent)
{
    fillWithTestData(5);

    buffer.resetElementsFrom(3);

    // Elements 0, 1 and 2 are left => 3 elements
    EXPECT_EQ(3U, buffer.getUsedSlots());
    EXPECT_FALSE(buffer.isEmpty());
    EXPECT_EQ(0U, buffer.peek(4).getNumberOfElements());
}

TEST_F(ByteArrayRingBufferTest, resetBuffers)
{
    fillWithTestData(10);

    EXPECT_EQ(0U, buffer.getFreeSlots());

    buffer.pop();
    uint8_t data[1] = {0xff};
    // append without clearing not-needed space in chunk
    EXPECT_TRUE(buffer.append(asSlice(data)));

    Slice<const uint8_t> temp = buffer.peek(9);
    EXPECT_EQ(temp[0], 0xff);
    EXPECT_OUTPOST_CONSTEXPR_ASSERTION_DEATH(
            temp[1], "Slice access out of bounds");  // not allowed to access...
    EXPECT_EQ(temp.begin()[1], 0x0a);                // old data

    buffer.pop();
    // append that sets everything to zero
    EXPECT_TRUE(buffer.append(asSlice(data), 0, true));
    temp = buffer.peek(9);
    EXPECT_EQ(temp[0], 0xff);
    EXPECT_OUTPOST_CONSTEXPR_ASSERTION_DEATH(
            temp[1], "Slice access out of bounds");  // not allowed to access...
    EXPECT_EQ(temp.begin()[1], 0x00);                // new data
}
