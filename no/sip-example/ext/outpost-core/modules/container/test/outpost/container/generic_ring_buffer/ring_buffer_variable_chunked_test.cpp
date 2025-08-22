/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/container/ring_buffer_best_effort_variable_chunked.h>
#include <outpost/container/ring_buffer_variable_chunked.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/harness.h>

#include <numeric>

using namespace outpost;
using namespace outpost::container;
using namespace ::testing;
using namespace std;

// ---------------------------------------------------------------------------
class VariableChunkedRingBufferTest : public ::testing::Test
{
public:
    static const uint32_t bufferSize = 100U;

    VariableChunkedRingBufferStorage<bufferSize, uint8_t> mBuffer;
};

const uint32_t VariableChunkedRingBufferTest::bufferSize;

// ---------------------------------------------------------------------------
TEST_F(VariableChunkedRingBufferTest, shouldReturnNumberOfFreeBytes1)
{
    uint8_t buffer[5];
    VariableChunkedRingBufferStorage<10, uint16_t> ringBuffer;
    ringBuffer.pushChunk(asSlice(buffer));

    EXPECT_THAT(ringBuffer.getFreeUserBytes(), 1);
}

TEST_F(VariableChunkedRingBufferTest, shouldReturnNumberOfFreeBytes2)
{
    uint8_t buffer[6];
    VariableChunkedRingBufferStorage<10, uint16_t> ringBuffer;
    ringBuffer.pushChunk(asSlice(buffer));

    EXPECT_THAT(ringBuffer.getFreeUserBytes(), 0);
}

TEST_F(VariableChunkedRingBufferTest, shouldReturnNumberOfFreeBytes3)
{
    uint8_t buffer[7];
    VariableChunkedRingBufferStorage<10, uint16_t> ringBuffer;
    ringBuffer.pushChunk(asSlice(buffer));

    EXPECT_THAT(ringBuffer.getFreeUserBytes(), 0);
}

TEST_F(VariableChunkedRingBufferTest, shouldBeInitalEmpty)
{
    EXPECT_THAT(mBuffer.getNumberOfChunks(), 0U);
    EXPECT_EQ(bufferSize, mBuffer.capacity());
    EXPECT_EQ(bufferSize, mBuffer.getFreeUserBytes() + decltype(mBuffer)::headerSize);
    EXPECT_EQ(0U, mBuffer.getAvailableBytes());
    EXPECT_TRUE(mBuffer.isEmpty());
}

TEST_F(VariableChunkedRingBufferTest, shouldInsertAChunk)
{
    uint8_t data[] = {1, 2};

    mBuffer.pushChunk(asSlice(data));

    EXPECT_THAT(mBuffer.getNumberOfChunks(), 1U);
}

TEST_F(VariableChunkedRingBufferTest, shouldResetBuffer)
{
    uint8_t data[] = {1, 2};

    mBuffer.pushChunk(asSlice(data));
    mBuffer.reset();

    EXPECT_THAT(mBuffer.getNumberOfChunks(), 0U);
    EXPECT_THAT(mBuffer.getAvailableBytes(), 0U);
}

TEST_F(VariableChunkedRingBufferTest, shouldPeekAChunk)
{
    uint8_t data[] = {1, 2};
    uint8_t output[] = {0, 0, 0, 0};

    mBuffer.pushChunk(asSlice(data));
    auto result = mBuffer.peekChunkInto(asSlice(output));

    ASSERT_THAT(result, IsTrue());
    ASSERT_THAT(result->getNumberOfElements(), 2U);
    EXPECT_ARRAY_EQ(uint8_t, result->getDataPointer(), data, 2);

    EXPECT_THAT(mBuffer.getNumberOfChunks(), 1U);
    EXPECT_THAT(mBuffer.getAvailableBytes(), 2U);
}

TEST_F(VariableChunkedRingBufferTest, shouldPopAChunk)
{
    uint8_t data[] = {1, 2};
    uint8_t output[] = {0, 0, 0, 0};

    mBuffer.pushChunk(asSlice(data));
    const auto result = mBuffer.popChunkInto(asSlice(output));

    ASSERT_THAT(result, IsTrue());
    ASSERT_THAT(result->getNumberOfElements(), 2U);
    EXPECT_ARRAY_EQ(uint8_t, result->getDataPointer(), data, 2);

    EXPECT_THAT(mBuffer.getNumberOfChunks(), 0U);
    EXPECT_THAT(mBuffer.getAvailableBytes(), 0U);
}

TEST_F(VariableChunkedRingBufferTest, shouldPushAndPopMultipleElements)
{
    uint8_t data[] = {1, 2, 3, 4};
    uint8_t output[] = {0, 0, 0, 0};

    mBuffer.pushChunk(asSlice(data));
    mBuffer.pushChunk(asSlice(data));

    {
        memset(output, 0U, sizeof(output));
        const auto result = mBuffer.popChunkInto(asSlice(output));
        ASSERT_THAT(result, IsTrue());
        ASSERT_THAT(result->getNumberOfElements(), 4);
        EXPECT_ARRAY_EQ(uint8_t, result->getDataPointer(), data, 4);
        EXPECT_THAT(mBuffer.isEmpty(), IsFalse());
    }

    {
        memset(output, 0U, sizeof(data));
        auto result = mBuffer.popChunkInto(asSlice(output));
        ASSERT_THAT(result, IsTrue());
        EXPECT_THAT(mBuffer.isEmpty(), IsTrue());
        ASSERT_THAT(result->getNumberOfElements(), 4);
        EXPECT_ARRAY_EQ(uint8_t, result->getDataPointer(), data, 4);
    }
}

TEST_F(VariableChunkedRingBufferTest, shouldWriteMoreThanRead)
{
    uint8_t data[] = {1, 2};
    uint8_t output[] = {0, 0, 0, 0};

    for (uint i = 0; i < 1000U; i++)
    {
        mBuffer.pushChunk(asSlice(data));
        mBuffer.pushChunk(asSlice(data));
        auto result = mBuffer.popChunkInto(asSlice(output));
        ASSERT_THAT(result, IsTrue());
        ASSERT_THAT(result->getNumberOfElements(), 2U);
    }

    // 33 msg max. Minus the last one read.
    static_assert(bufferSize == 100U, "Test needs to be adjusted!");
    EXPECT_THAT(mBuffer.getNumberOfChunks(), 32U);
    // 99 bytes are max used. Minus the last read Chunk of 3 bytes.
    EXPECT_THAT(mBuffer.getAvailableBytes(), 32U * 2U);
}

TEST_F(VariableChunkedRingBufferTest, shouldWriteLargeChunk)
{
    uint8_t data[] = {1, 2};

    // fill buffer and overflood a bit
    static_assert(bufferSize == 100U, "Test needs to be adjusted!");
    for (uint i = 0; i < 33 + 5; i++)
    {
        mBuffer.pushChunk(asSlice(data));
    }

    uint8_t largeData[bufferSize - decltype(mBuffer)::headerSize];
    auto ret = mBuffer.pushChunk(asSlice(largeData));

    EXPECT_THAT(mBuffer.getNumberOfChunks(), 1U);
    EXPECT_THAT(mBuffer.getAvailableBytes(), sizeof(largeData));
    EXPECT_THAT(ret, sizeof(largeData));
}

TEST_F(VariableChunkedRingBufferTest, shouldNotWriteTooLargeChunk)
{
    uint8_t data[] = {1, 2};

    static_assert(bufferSize == 100U, "Test needs to be adjusted!");
    // fill buffer and overflood a bit
    for (uint i = 0; i < 33 + 5; i++)
    {
        mBuffer.pushChunk(asSlice(data));
    }

    uint8_t largeData[bufferSize - decltype(mBuffer)::headerSize + 1U];
    auto old = mBuffer.getNumberOfChunks();
    auto ret = mBuffer.pushChunk(asSlice(largeData));

    EXPECT_THAT(mBuffer.getNumberOfChunks(), old);
    EXPECT_THAT(ret, 0U);
}

TEST_F(VariableChunkedRingBufferTest, shouldNotPopChunkIntoIfOutputBufferIsTooSmall)
{
    uint8_t data[] = {1, 2};
    uint8_t output[1];

    mBuffer.pushChunk(asSlice(data));
    auto result = mBuffer.popChunkInto(asSlice(output));

    EXPECT_THAT(result, IsFalse());
    EXPECT_THAT(result.error(), Eq(decltype(mBuffer)::OperationalResult::bufferTooSmall));
    EXPECT_THAT(mBuffer.getNumberOfChunks(), 1U);
    EXPECT_THAT(mBuffer.getAvailableBytes(), 2U);
}

TEST_F(VariableChunkedRingBufferTest, shouldNotpeekChunkIntoIfOutputBufferIsTooSmall)
{
    uint8_t data[] = {1, 2};
    uint8_t output[1];

    mBuffer.pushChunk(asSlice(data));
    auto result = mBuffer.peekChunkInto(asSlice(output));
    EXPECT_THAT(result, IsFalse());
    EXPECT_THAT(result.error(), Eq(decltype(mBuffer)::OperationalResult::bufferTooSmall));
    EXPECT_THAT(mBuffer.getNumberOfChunks(), 1U);
    EXPECT_THAT(mBuffer.getAvailableBytes(), 2U);
}

TEST_F(VariableChunkedRingBufferTest, outputSliceShouldBeSmallerThenInputSliceOnPop)
{
    uint8_t data[] = {1, 2};
    uint8_t output[3];

    mBuffer.pushChunk(asSlice(data));
    auto result = mBuffer.popChunkInto(asSlice(output));

    ASSERT_THAT(result, IsTrue());
    ASSERT_THAT(result->getNumberOfElements(), sizeof(data));
    EXPECT_THAT(mBuffer.getNumberOfChunks(), 0U);
    EXPECT_THAT(mBuffer.getAvailableBytes(), 0U);
}

TEST_F(VariableChunkedRingBufferTest, outputSliceShouldBeSmallerThenInputSliceOnPeek)
{
    uint8_t data[] = {1, 2};
    uint8_t output[3];

    mBuffer.pushChunk(asSlice(data));
    auto result = mBuffer.peekChunkInto(asSlice(output));

    ASSERT_THAT(result, IsTrue());
    ASSERT_THAT(result->getNumberOfElements(), sizeof(data));
    EXPECT_THAT(mBuffer.getNumberOfChunks(), 1U);
    EXPECT_THAT(mBuffer.getAvailableBytes(), 2U);
}

TEST_F(VariableChunkedRingBufferTest, shouldAddElementWithMaxSize)
{
    VariableChunkedRingBufferStorage<Limits<uint8_t>::max * 2, uint8_t> bufferUint8_t;
    uint8_t element[Limits<uint8_t>::max];
    memset(element, 0U, sizeof(element));

    bufferUint8_t.pushChunk(asSlice(element));

    EXPECT_THAT(bufferUint8_t.getNumberOfChunks(), 1U);
}

TEST_F(VariableChunkedRingBufferTest, shouldNotWriteChunkWhichExceedsMaxChunkSize)
{
    VariableChunkedRingBufferStorage<Limits<uint8_t>::max * 2, uint8_t> bufferUint8_t;
    uint8_t element[Limits<uint8_t>::max + 1];
    memset(element, 0U, sizeof(element));

    auto ret = bufferUint8_t.pushChunk(asSlice(element));
    EXPECT_THAT(ret, 0U);
    EXPECT_THAT(mBuffer.getNumberOfChunks(), Eq(0U));
}

TEST_F(VariableChunkedRingBufferTest, shouldNotCreateRingBufferFromATooSmallStorage)
{
    uint8_t storage[1];

    (void) storage;
    EXPECT_OUTPOST_CONSTEXPR_ASSERTION_DEATH(
            VariableChunkedRingBuffer<uint16_t> bufferUint16_t(asSlice(storage)),
            "Must be possible to write at least one header!");
}

TEST_F(VariableChunkedRingBufferTest, shouldAddElementWhichSizeNeedsToBeStoredInTwoBytes)
{
    VariableChunkedRingBufferStorage<Limits<uint8_t>::max * 4, uint32_t> bufferUint32_t;
    uint8_t element[301];  // 1 | 00101101
    bufferUint32_t.pushChunk(asSlice(element));

    auto result = bufferUint32_t.popChunkInto(asSlice(element));
    ASSERT_THAT(result, IsTrue());
    EXPECT_THAT(result->getNumberOfElements(), 301);
}

TEST_F(VariableChunkedRingBufferTest, shouldAddConstData)
{
    const uint8_t element[] = {1, 2, 3};
    mBuffer.pushChunk(asSlice(element));
}

TEST_F(VariableChunkedRingBufferTest, shouldAddEmptyChunk)
{
    mBuffer.pushChunk(Slice<uint8_t>::empty());
    EXPECT_THAT(mBuffer.getNumberOfChunks(), 1U);

    mBuffer.peekChunkInto(Slice<uint8_t>::empty());
    EXPECT_THAT(mBuffer.getNumberOfChunks(), 1U);

    mBuffer.popChunkInto(Slice<uint8_t>::empty());
    EXPECT_THAT(mBuffer.getNumberOfChunks(), 0U);
}

TEST_F(VariableChunkedRingBufferTest, shouldNotRaiseAssertionWhenPopPeekOrDropCalledOnEmptyBuffer)
{
    uint8_t readBuffer[] = {0};
    mBuffer.popChunkInto(asSlice(readBuffer));
    mBuffer.peekChunkInto(asSlice(readBuffer));
    mBuffer.discardChunk();
}

TEST(BestEffortVariableChunkedRingBufferStorage, shouldSelectBestHeaderType)
{
    using Type1 = BestEffortVariableChunkedRingBufferStorage<3, Limits<uint8_t>::max>;
    static_assert(std::is_same<Type1::Type, uint8_t>::value, "");
    using Type2 = BestEffortVariableChunkedRingBufferStorage<3, Limits<uint8_t>::max + 1>;
    static_assert(std::is_same<Type2::Type, uint16_t>::value, "");
    using Type3 = BestEffortVariableChunkedRingBufferStorage<3, Limits<uint16_t>::max>;
    static_assert(std::is_same<Type3::Type, uint16_t>::value, "");
    using Type4 = BestEffortVariableChunkedRingBufferStorage<3, Limits<uint16_t>::max + 1>;
    static_assert(std::is_same<Type4::Type, uint32_t>::value, "");
    using Type5 = BestEffortVariableChunkedRingBufferStorage<3, Limits<uint32_t>::max>;
    static_assert(std::is_same<Type5::Type, uint32_t>::value, "");
    using Type6 =
            BestEffortVariableChunkedRingBufferStorage<3,
                                                       static_cast<uint64_t>(Limits<uint32_t>::max)
                                                               + 1>;
    static_assert(std::is_same<Type6::Type, uint64_t>::value, "");
}

TEST(BestEffortVariableChunkedRingBufferStorage, ReadWriteOverBoundaryWrap)
{
    constexpr size_t minNumOfPackets = 128;
    constexpr size_t maxPacketSize = 1472;
    constexpr size_t defaultPacketSize = 54;

    BestEffortVariableChunkedRingBufferStorage<minNumOfPackets, maxPacketSize> buffer;

    // buffer size in bytes should be how I think it is
    EXPECT_EQ(buffer.capacity(), minNumOfPackets * (maxPacketSize + sizeof(uint16_t)));

    array<uint8_t, defaultPacketSize> defaultPacket;
    iota(defaultPacket.begin(), defaultPacket.end(), 0);
    array<uint8_t, maxPacketSize> bigPacket;
    bigPacket.fill('A');
    // we now have a small buffer with continuously increasing values
    // and a big buffer with 'A's

    // Fill it up to roughly half, alternating pattern
    unsigned pushedChunks = 0;
    while (buffer.getFreeUserBytes() > buffer.capacity() / 2)
    {
        EXPECT_EQ(buffer.pushChunk(asSlice(defaultPacket)), defaultPacket.size());
        pushedChunks++;
        EXPECT_EQ(buffer.pushChunk(asSlice(bigPacket)), bigPacket.size());
        pushedChunks++;
    }

    EXPECT_EQ(buffer.getNumberOfChunks(), pushedChunks);

    // Pop all but one pair
    while (buffer.getNumberOfChunks() > 2)
    {
        array<uint8_t, maxPacketSize> readBuffer;
        const auto maybeSmallPacket = buffer.popChunkInto(asSlice(readBuffer));
        ASSERT_TRUE(maybeSmallPacket);
        EXPECT_TRUE(SliceMatch(*maybeSmallPacket, asSlice(defaultPacket)));
        pushedChunks--;

        const auto maybeBigPacket = buffer.popChunkInto(asSlice(readBuffer));
        ASSERT_TRUE(maybeBigPacket);
        EXPECT_TRUE(SliceMatch(*maybeBigPacket, asSlice(bigPacket)));
        pushedChunks--;
    }

    EXPECT_EQ(pushedChunks, 2U);
    EXPECT_EQ(buffer.getNumberOfChunks(), 2U);

    // The buffer has still two elements in the center. Now
    // fill it up (nearly) completely, while going over boundaries
    while (buffer.getFreeUserBytes() > (defaultPacket.size() + bigPacket.size()))
    {
        EXPECT_EQ(buffer.pushChunk(asSlice(defaultPacket)), defaultPacket.size());
        pushedChunks++;
        EXPECT_EQ(buffer.pushChunk(asSlice(bigPacket)), bigPacket.size());
        pushedChunks++;
    }

    EXPECT_GT(buffer.getNumberOfChunks(), minNumOfPackets);

    // Pop all
    while (buffer.getNumberOfChunks() > 0)
    {
        array<uint8_t, maxPacketSize> readBuffer;
        const auto maybeSmallPacket = buffer.popChunkInto(asSlice(readBuffer));
        ASSERT_TRUE(maybeSmallPacket);
        EXPECT_TRUE(SliceMatch(*maybeSmallPacket, asSlice(defaultPacket)));
        pushedChunks--;

        const auto maybeBigPacket = buffer.popChunkInto(asSlice(readBuffer));
        ASSERT_TRUE(maybeBigPacket);
        EXPECT_TRUE(SliceMatch(*maybeBigPacket, asSlice(bigPacket)));
        pushedChunks--;
    }

    EXPECT_EQ(pushedChunks, 0U);
    EXPECT_EQ(buffer.getNumberOfChunks(), 0U);
}
