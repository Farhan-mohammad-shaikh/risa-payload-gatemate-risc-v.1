/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2023-2024, Janosch Reinking
 * Copyright (c) 2024, Cold, Erin Cynthia
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
 * \author  Pascal Pieper
 */

#include "ring_buffer_testinterfaces.h"

#include <cxxabi.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/container/ring_buffer_allocators/file_backed_ring_buffer.h>
#include <unittest/container/ring_buffer_helper.h>
#include <unittest/harness.h>

#include <math.h>

#include <algorithm>

using namespace outpost;
using namespace outpost::container;
using namespace unittest::container;
using OperationResult = RingBufferBase::OperationResult;
using namespace outpost::container::ring_buffer;
using namespace testing;

// Helper function for printing classnames
static std::string
demangled(char const* tname)
{
    std::unique_ptr<char, void (*)(void*)> name{abi::__cxa_demangle(tname, 0, 0, nullptr),
                                                std::free};
    return {name.get()};
}

// Helper class to print Testnames in GTEST output
class RingBufferTestParameterNamer
{
public:
    template <class ParamType>
    static std::string
    GetName(int /* num */)
    {
        return demangled(typeid(typename ParamType::RingbufferAllocatorType).name())
                //+ std::to_string(num);
                ;
    }
};

// ----- End of helpers

// The parameter type for the TYPED_TESTs. Contains an Allocator and an expected size.
template <typename A, size_t size>
struct RingBufferTestParameter
{
    using RingbufferAllocatorType = A;
    static constexpr size_t expectedSize = size;
};

/**
 * @brief Class with ownership of the Ringbuffers with certain Allocators.
 * The `RingBufferTestInterface` is the Ringbuffer, but also with
 * the (runtime) Allocator instance accessible by the tests.
 *
 * This default implementation assumes that the allocator comes with its own storage.
 */
template <class RingbufferAllocator, size_t size, typename = void>
struct RingBufferHolder
{
    using AllocatorType = RingbufferAllocator;

    class RingBufferTestInterface : public RingBuffer<AllocatorType, uint8_t>
    {
    public:
        // make allocator accessible for tests.
        // `AllocatorStorage` refers to the the Allocator itself, not the actual
        // buffer data storage.
        using AllocatorStorage<RingbufferAllocator>::getAllocator;

        // make appendPaddingElements() available for tests
        using RingBuffer<AllocatorType, uint8_t>::appendPaddingElements;

        // pass parameter to actual class
        explicit RingBufferTestInterface() : RingBuffer<AllocatorType, uint8_t>()
        {
        }
    };

    // constructor to give `mActualStorage` to ReferencedArrayAllocator
    explicit RingBufferHolder() : mRingBufferTestInterface()
    {
    }

    // cppcheck-suppress initializerList ; False positive: It is the only member
    RingBufferTestInterface mRingBufferTestInterface;
};

/**
 * @brief RingBufferHolder where the allocator needs external buffer.
 * This owns and passes the raw buffer behind the RingBuffer.
 */
template <class RingbufferAllocator, size_t size>
struct RingBufferHolder<
        RingbufferAllocator,
        size,
        typename std::enable_if<std::is_base_of<ReferencedArrayAllocatorTestInterface,
                                                RingbufferAllocator>::value>::type>
{
    using AllocatorType = RingbufferAllocator;

    class RingBufferTestInterface : public RingBuffer<AllocatorType, uint8_t>
    {
    public:
        // make allocator accessible for tests.
        // `AllocatorStorage` refers to the the Allocator itself, not the actual
        // buffer data storage.
        using AllocatorStorage<RingbufferAllocator>::getAllocator;

        // make appendPaddingElements() available for tests
        using RingBuffer<AllocatorType, uint8_t>::appendPaddingElements;

        // pass parameter to actual class
        explicit RingBufferTestInterface(Slice<uint8_t> mActualStorage) :
            RingBuffer<AllocatorType, uint8_t>(mActualStorage)
        {
        }
    };

    // constructor to give `mActualStorage` to ReferencedArrayAllocator
    // cppcheck-suppress initializerList ; Ignored for test-code
    RingBufferHolder() : mActualStorage(), mRingBufferTestInterface(asSlice(mActualStorage))
    {
    }

    std::array<uint8_t, size> mActualStorage;

    RingBufferTestInterface mRingBufferTestInterface;
};

// The GTEST fixture, which is a RingBufferHolder and a Test.
template <class RingBufferTestParameter>
class RingBufferTest
    : public testing::Test,
      public RingBufferHolder<typename RingBufferTestParameter::RingbufferAllocatorType,
                              RingBufferTestParameter::expectedSize>
{
public:
    static constexpr size_t bufferSize = RingBufferTestParameter::expectedSize;
    using AllocatorType = typename RingBufferTestParameter::RingbufferAllocatorType;
    using RingbufferType = RingBuffer<AllocatorType, uint8_t>;

    RingBufferTest() : RingBufferHolder<AllocatorType, bufferSize>()
    {
        // std::cout << "Ringbuffer Constructor with "
        //     << demangled(typeid(RingbufferType).name())
        //     << std::endl;
    }

    void
    fillWithTestData(const size_t count)
    {
        std::vector<uint8_t> numbers;
        size_t value = 0;
        for (size_t index = 0; index < count; index += 2)
        {
            numbers.emplace_back(value);
            numbers.emplace_back(value + 10);
            value++;
        }
        EXPECT_EQ(this->mRingBufferTestInterface.append(asSlice(numbers).first(count)),
                  OperationResult::success);
    }
};

// "static storage" of constexpr values
template <class RingBufferTestParameter>
constexpr size_t RingBufferTest<RingBufferTestParameter>::bufferSize;

constexpr size_t ringBufferTestingSize = 10;

// The list of Ringbuffer-Allocators that offer "Direct Access".
// This allows the (test of) no-copy operations `pop(...)` and `peek(...)`
using DirectAccessAllocatorImplementations = ::testing::Types<
        RingBufferTestParameter<ReferencedArrayAllocatorTestInterface,  // Look mum, no template
                                                                        // argument!
                                ringBufferTestingSize>,
        RingBufferTestParameter<ReferencedArrayAllocatorMockMetadataAccessTestInterface,
                                ringBufferTestingSize>,
        RingBufferTestParameter<ReferencedArrayAllocatorMockEverythingTestInterface,
                                ringBufferTestingSize>>;

// List of Allocators for test of the default RingBuffer functions.
// The main difference is the use of `popInto(...)` and `peekInto(...)`
using CopyAllocatorImplementations = ::testing::Types<
        RingBufferTestParameter<ReferencedArrayAllocatorTestInterface, ringBufferTestingSize>,
        RingBufferTestParameter<ReferencedArrayAllocatorMockMetadataAccessTestInterface,
                                ringBufferTestingSize>,
        RingBufferTestParameter<ReferencedArrayAllocatorMockEverythingTestInterface,
                                ringBufferTestingSize>,
        RingBufferTestParameter<FileBackedAllocatorTestInterface<ringBufferTestingSize>,
                                ringBufferTestingSize>>;

// --- start of actual tests

TYPED_TEST_SUITE(RingBufferTest, CopyAllocatorImplementations, RingBufferTestParameterNamer);

TYPED_TEST(RingBufferTest, initialValues)
{
    EXPECT_EQ(TestFixture::bufferSize, TestFixture::mRingBufferTestInterface.capacity());
    EXPECT_EQ(0U, TestFixture::mRingBufferTestInterface.getAvailableElements());
    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty());
}

TYPED_TEST(RingBufferTest, rejectOverlongData)
{
    uint8_t data[TestFixture::bufferSize + 1];
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(asSlice(data)),
              OperationResult::notEnoughSpace);
}

TYPED_TEST(RingBufferTest, writeIncreasesElementCount)
{
    uint8_t twoElements[2] = {10, 11};
    const auto twoElementsSlice = asSlice(twoElements);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(twoElementsSlice.first(1)),
              OperationResult::success);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(twoElementsSlice.first(1)),
              OperationResult::success);
    EXPECT_EQ(2U, TestFixture::mRingBufferTestInterface.getAvailableElements());

    EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(twoElementsSlice),
              OperationResult::success);
    EXPECT_EQ(4U, TestFixture::mRingBufferTestInterface.getAvailableElements());
}

TYPED_TEST(RingBufferTest, appendIsRejectedAfterBufferIsFull)
{
    uint8_t element[1] = {12};

    while (!TestFixture::mRingBufferTestInterface.isFull())
    {
        EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(asSlice(element)),
                  OperationResult::success);
    }

    EXPECT_EQ(TestFixture::bufferSize,
              TestFixture::mRingBufferTestInterface.getAvailableElements());
    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isFull());

    EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(asSlice(element)),
              OperationResult::notEnoughSpace);
    EXPECT_EQ(TestFixture::bufferSize,
              TestFixture::mRingBufferTestInterface.getAvailableElements());
}

TYPED_TEST(RingBufferTest, peekIntoPopIntoWithZeroElementsIsSane)
{
    // first, append nothing and expect a success
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(Slice<uint8_t>::empty()),
              OperationResult::success);

    // Changed behavior: If requested zero elements, it should return ::success,
    // because all requested elements (zero) were copied successfully.
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.peekInto(Slice<uint8_t>::empty()),
              OperationResult::success);

    EXPECT_EQ(TestFixture::mRingBufferTestInterface.popInto(Slice<uint8_t>::empty()),
              OperationResult::success);

    // Fill with one element
    uint8_t element[1] = {0};
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(asSlice(element)),
              OperationResult::success);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), 1U);

    // Request zero elements, get zero elements (successfully)
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.peekInto(Slice<uint8_t>::empty()),
              OperationResult::success);

    // Request zero elements, get zero elements (successfully)
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.popInto(Slice<uint8_t>::empty()),
              OperationResult::success);

    // unchanged number of elements waiting
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), 1U);
}

TYPED_TEST(RingBufferTest, appendIsRejectedIfNotFullButNotEnoughSpace)
{
    const auto firstFillElements = TestFixture::bufferSize / 2;
    TestFixture::fillWithTestData(firstFillElements);
    EXPECT_EQ(firstFillElements, TestFixture::mRingBufferTestInterface.getAvailableElements());

    uint8_t tooBig[(TestFixture::bufferSize - firstFillElements) + 1];
    memset(tooBig, 0, sizeof(tooBig));
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(asSlice(tooBig)),
              OperationResult::notEnoughSpace);
    EXPECT_EQ(firstFillElements, TestFixture::mRingBufferTestInterface.getAvailableElements());
}

TYPED_TEST(RingBufferTest, getContinuousElementsIsSane)
{
    auto& ringbuffer = TestFixture::mRingBufferTestInterface;

    const auto half = TestFixture::bufferSize / 2;
    const auto quarter = half / 2;
    const auto all = TestFixture::bufferSize;
    const auto more = TestFixture::bufferSize + 1;

    // test with empty buffer
    //  head
    //    |
    //  -[ | | | | | ]-
    //    |
    //  tail
    EXPECT_EQ(ringbuffer.getAvailableElements(), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(quarter), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(half), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(all), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(more), 0U);

    // half filled
    // test with empty buffer
    //         head
    //          |
    //  -[x|x|x| | | ]-
    //    |
    //  tail
    TestFixture::fillWithTestData(half);

    EXPECT_EQ(ringbuffer.getAvailableElements(), half);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(), half);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(quarter), half - quarter);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(half), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(half - 1), 1U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(all), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(more), 0U);

    // fully filled
    //   head
    //    |
    //  -[x|x|x|x|x|x]-
    //    |
    //  tail
    TestFixture::fillWithTestData(all - half);

    EXPECT_EQ(ringbuffer.getAvailableElements(), all);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(), all);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(quarter), all - quarter);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(half), all - half);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(all - 1), 1U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(all), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(more), 0U);

    // _last_ half filled
    //        head
    //          |
    //  -[ | | |x|x|x]-
    //    |
    //   tail
    ASSERT_EQ(ringbuffer.discardElements(half), OperationResult::success);

    EXPECT_EQ(ringbuffer.getAvailableElements(), half);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(), half);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(quarter), half - quarter);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(half), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(all), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(more), 0U);

    // fully filled, but over boundary
    //         head
    //          |
    //  -[x|x|x|x|x|x]-
    //          |
    //        tail
    TestFixture::fillWithTestData(all - half);

    EXPECT_EQ(ringbuffer.getAvailableElements(), all);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(), half);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(quarter), half - quarter);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(half), half);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(half - 1), 1U);  // still before boundary
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(all), 0U);
    EXPECT_EQ(ringbuffer.getAvailableContinuousElements(more), 0U);
}

TYPED_TEST(RingBufferTest, retrieveAndVerifyElements)
{
    const auto readOperations = TestFixture::bufferSize / 2;
    TestFixture::fillWithTestData(readOperations * 2);

    for (unsigned i = 0; i < readOperations; ++i)
    {
        EXPECT_FALSE(TestFixture::mRingBufferTestInterface.isEmpty());
        std::array<uint8_t, 2> twoElementChunk;
        ASSERT_EQ(OperationResult::success,
                  TestFixture::mRingBufferTestInterface.popInto(asSlice(twoElementChunk)));

        EXPECT_EQ(i, twoElementChunk[0]) << "Error at read nr. " << i;
        EXPECT_EQ(i + 10, twoElementChunk[1]) << "Error at read nr. " << i;
    }
    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty());
}

TYPED_TEST(RingBufferTest, readAndWriteOverBoundaries)
{
    const auto chunkSize = 3;
    std::array<uint8_t, chunkSize> expected;
    std::array<uint8_t, chunkSize> actual;

    // prepare inital content
    const auto prefillChunks = TestFixture::bufferSize / (chunkSize);
    for (unsigned i = 0; i < prefillChunks; i++)
    {
        std::fill(expected.begin(), expected.end(), i);
        EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(asSlice(expected)),
                  OperationResult::success);
    }

    // slide window through TestFixture::mRingBufferTestInterface, possibly hitting boundaries
    unsigned chunkNr = 0;
    for (; chunkNr < TestFixture::bufferSize * chunkSize; chunkNr++)
    {
        // read a chunk
        std::fill(expected.begin(), expected.end(), chunkNr);
        EXPECT_EQ(TestFixture::mRingBufferTestInterface.popInto(asSlice(actual)),
                  OperationResult::success);
        EXPECT_EQ(expected, actual);

        // write a chunk
        std::fill(expected.begin(), expected.end(), chunkNr + prefillChunks);
        EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(asSlice(expected)),
                  OperationResult::success);
    }

    // read tail
    for (unsigned i = 0; i < prefillChunks; i++)
    {
        std::fill(expected.begin(), expected.end(), chunkNr++);
        EXPECT_EQ(TestFixture::mRingBufferTestInterface.popInto(asSlice(actual)),
                  OperationResult::success);
        EXPECT_EQ(expected, actual);
    }

    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty());
}

TYPED_TEST(RingBufferTest, peekInto)
{
    TestFixture::fillWithTestData(TestFixture::bufferSize);

    for (size_t i = 0; i < TestFixture::bufferSize / 2; i += 2)
    {
        uint8_t element[2];
        ASSERT_EQ(TestFixture::mRingBufferTestInterface.peekInto(asSlice(element), i * 2),
                  OperationResult::success);
        EXPECT_EQ(i, element[0]);
        EXPECT_EQ(i + 10, element[1]);

        EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(),
                  TestFixture::bufferSize);
    }
}

TYPED_TEST(RingBufferTest, invalidPeekIntoWouldUnderflow)
{
    uint8_t buf[TestFixture::bufferSize];

    // is empty
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.peekInto(asSlice(buf),
                                                             TestFixture::bufferSize + 1),
              OperationResult::readWriteTooBig);

    TestFixture::fillWithTestData(TestFixture::bufferSize);

    // contains something, but read is completely outside boundary
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.peekInto(asSlice(buf),
                                                             TestFixture::bufferSize + 1),
              OperationResult::readWriteTooBig);

    // contains something, but last element is outside boundary
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.peekInto(asSlice(buf), 1),
              OperationResult::readWriteTooBig);
}

TYPED_TEST(RingBufferTest, resetDropsAllContent)
{
    TestFixture::fillWithTestData(TestFixture::bufferSize);

    TestFixture::mRingBufferTestInterface.reset();

    EXPECT_EQ(TestFixture::bufferSize, TestFixture::mRingBufferTestInterface.getFreeElements());
    EXPECT_EQ(0U, TestFixture::mRingBufferTestInterface.getAvailableElements());
    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty());

    uint8_t element[1];
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.popInto(asSlice(element)),
              OperationResult::readWriteTooBig);
}

TYPED_TEST(RingBufferTest, WasEveryMemberFunctionAccessed)
{
    TestFixture::fillWithTestData(TestFixture::bufferSize);

    while (true)
    {
        uint8_t dummy[1];
        const auto r = TestFixture::mRingBufferTestInterface.popInto(asSlice(dummy));
        if (r != OperationResult::success)
        {
            EXPECT_EQ(r, OperationResult::readWriteTooBig);
            EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), 0U);
            break;
        }
    }
    TestFixture::mRingBufferTestInterface.reset();  // also calls `setReadIndex()

    ASSERT_TRUE(TestFixture::mRingBufferTestInterface.getAllocator().wasEverythingAccessed())
            << "Not all provided functions were accessed";
}

TYPED_TEST(RingBufferTest, ResetFromDiscardsEndElements)
{
    auto resetAfter = TestFixture::bufferSize / 2;
    resetAfter += resetAfter % 2;  // in case "half bufferSize" is not divisible by two, round up
    const auto readOperations = (TestFixture::bufferSize - resetAfter) / 2;
    ASSERT_EQ(resetAfter + readOperations * 2, TestFixture::bufferSize) << "Test setup is invalid";

    EXPECT_FALSE(TestFixture::mRingBufferTestInterface.resetAfter(1));  // reset on empty buffer

    TestFixture::fillWithTestData(readOperations * 2);  // fill
    EXPECT_TRUE(
            TestFixture::mRingBufferTestInterface.resetAfter(readOperations));  // reset last half

    for (unsigned i = 0; i < readOperations / 2; ++i)
    {
        EXPECT_FALSE(TestFixture::mRingBufferTestInterface.isEmpty());
        std::array<uint8_t, 2> twoElementChunk;
        ASSERT_EQ(OperationResult::success,
                  TestFixture::mRingBufferTestInterface.popInto(asSlice(twoElementChunk)));

        EXPECT_EQ(i, twoElementChunk[0]) << "Error at read nr. " << i;
        EXPECT_EQ(i + 10, twoElementChunk[1]) << "Error at read nr. " << i;
    }

    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty())
            << "Buffer still has " << TestFixture::mRingBufferTestInterface.getAvailableElements()
            << " elements";
}

TYPED_TEST(RingBufferTest, SkipActuallySkipsElementsToWrite)
{
    auto skipElements = TestFixture::bufferSize / 2;
    skipElements +=
            skipElements % 2;  // in case "half bufferSize" is not divisible by two, round up
    const auto readOperations = (TestFixture::bufferSize - skipElements) / 2;
    ASSERT_EQ(skipElements + readOperations * 2, TestFixture::bufferSize) << "Test setup is "
                                                                             "invalid";

    // zero out mRingBufferTestInterface
    {
        std::array<uint8_t, 1> dummy = {};
        while (TestFixture::mRingBufferTestInterface.append(asSlice(dummy))
               == OperationResult::success)
            ;
        EXPECT_EQ(TestFixture::mRingBufferTestInterface.getFreeElements(), 0U);
    }

    TestFixture::mRingBufferTestInterface.reset();
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), 0U);

    // skip zero elements (leaves state unchanged)
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.appendPaddingElements(0),
              OperationResult::success);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), 0U);

    // skip over half, should still be zero
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.appendPaddingElements(skipElements),
              OperationResult::success);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), skipElements);

    // skip more than available space
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.appendPaddingElements(skipElements + 1),
              OperationResult::notEnoughSpace);

    // fill rest half with different data
    TestFixture::fillWithTestData(readOperations * 2);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getFreeElements(), 0U);

    // First half should still contain zero from first operations
    for (unsigned i = 0; i < skipElements; ++i)
    {
        std::array<uint8_t, 1> dummy;
        dummy[0] = 0xff;  // just to make sure that it is differently initialized than expected
        EXPECT_EQ(TestFixture::mRingBufferTestInterface.popInto(asSlice(dummy)),
                  OperationResult::success);
        EXPECT_EQ(dummy[0], 0x00) << "Element not skipped in write";
    }

    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), readOperations * 2);

    for (unsigned i = 0; i < readOperations; i++)
    {
        EXPECT_FALSE(TestFixture::mRingBufferTestInterface.isEmpty());
        std::array<uint8_t, 2> twoElementChunk;
        ASSERT_EQ(OperationResult::success,
                  TestFixture::mRingBufferTestInterface.popInto(asSlice(twoElementChunk)));

        EXPECT_EQ(i, twoElementChunk[0]) << "Error at read nr. " << i;
        EXPECT_EQ(i + 10, twoElementChunk[1]) << "Error at read nr. " << i;
    }
    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty())
            << "Buffer still has " << TestFixture::mRingBufferTestInterface.getAvailableElements()
            << " elements";
}

TYPED_TEST(RingBufferTest, discardElementsActuallyDiscardsElementsToRead)
{
    TestFixture::fillWithTestData(TestFixture::bufferSize);

    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(),
              TestFixture::bufferSize);

    // discard too many elements
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.discardElements(TestFixture::bufferSize + 1),
              OperationResult::readWriteTooBig);

    // discard only one pair
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.discardElements(2), OperationResult::success);

    // peek at remaining
    for (size_t i = 1; i < TestFixture::bufferSize / 2; i += 2)
    {
        uint8_t element[2];
        ASSERT_EQ(TestFixture::mRingBufferTestInterface.peekInto(asSlice(element), (i - 1) * 2),
                  OperationResult::success);
        EXPECT_EQ(i, element[0]);
        EXPECT_EQ(i + 10, element[1]);
    }

    // discard rest
    const auto remainingElements = TestFixture::mRingBufferTestInterface.getAvailableElements();
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.discardElements(remainingElements),
              OperationResult::success);

    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), 0U);
}

template <typename T>
struct DirectAccessRingBufferTest : public RingBufferTest<T>
{
};

TYPED_TEST_SUITE(DirectAccessRingBufferTest,
                 DirectAccessAllocatorImplementations,
                 RingBufferTestParameterNamer);

// extra-functions
TYPED_TEST(DirectAccessRingBufferTest, peek)
{
    TestFixture::fillWithTestData(TestFixture::bufferSize);

    for (size_t i = 0; i < TestFixture::bufferSize / 2; i += 2)
    {
        const auto slice = TestFixture::mRingBufferTestInterface.peek(2, i * 2);
        EXPECT_EQ(slice.getNumberOfElements(), 2U);
        EXPECT_EQ(i, slice[0]);
        EXPECT_EQ(i + 10, slice[1]);

        EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(),
                  TestFixture::bufferSize);
    }
}

TYPED_TEST(DirectAccessRingBufferTest, peekAndPopWithZeroElementsIsSane)
{
    // Expected behavior: If RingBuffer is empty, always return ::empty.
    // Even when we only requested zero elements.
    const auto peekOnEmpty = TestFixture::mRingBufferTestInterface.peek(0);
    EXPECT_EQ(peekOnEmpty.getNumberOfElements(), 0U);

    const auto popOnEmpty = TestFixture::mRingBufferTestInterface.pop(0);
    EXPECT_EQ(popOnEmpty.getNumberOfElements(), 0U);

    // Fill with one element
    uint8_t element[1] = {0};
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(asSlice(element)),
              OperationResult::success);
    EXPECT_THAT(TestFixture::mRingBufferTestInterface.getAvailableContinuousElements(), Eq(1U));
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), 1U);

    // Request zero elements, get zero elements (successfully)
    const auto peekZero = TestFixture::mRingBufferTestInterface.peek(0);
    EXPECT_EQ(peekZero.getNumberOfElements(), 0U);

    // Request zero elements, get zero elements (successfully)
    const auto popZero = TestFixture::mRingBufferTestInterface.pop(0);
    EXPECT_EQ(popZero.getNumberOfElements(), 0U);

    // unchanged number of elements waiting
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableContinuousElements(), 1U);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), 1U);
}

TYPED_TEST(DirectAccessRingBufferTest, PeekWithUnderflow)
{
    // is empty
    {
        const auto ret = TestFixture::mRingBufferTestInterface.peek(1, 0);
        EXPECT_EQ(ret.getNumberOfElements(), 0U);
    }

    TestFixture::fillWithTestData(TestFixture::bufferSize);
    {
        // contains something, but read is completely outside boundary
        const auto ret = TestFixture::mRingBufferTestInterface.peek(1, TestFixture::bufferSize + 1);
        EXPECT_EQ(ret.getNumberOfElements(), 0U);
    }

    {
        // contains something, but last element is outside boundary
        const auto ret = TestFixture::mRingBufferTestInterface.peek(TestFixture::bufferSize, 1);
        EXPECT_EQ(ret.getNumberOfElements(), TestFixture::bufferSize - 1);
    }
}

TYPED_TEST(DirectAccessRingBufferTest, PeekWithMisalinged)
{
    // Test for: contains something, but returned TestFixture::mRingBufferTestInterface would be
    // fragmented i.e. read over boundary

    // fill first half
    TestFixture::fillWithTestData(TestFixture::bufferSize / 2);
    // use it up again
    auto slice = TestFixture::mRingBufferTestInterface.pop(TestFixture::bufferSize);
    EXPECT_EQ(slice.getNumberOfElements(), TestFixture::bufferSize / 2);

    // Read/Write pointer should now be in the middle, but otherwise empty
    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty());
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(), 0U);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableContinuousElements(), 0U);

    // For the test we want incrementing values in complete buffer, but fragmented.
    for (uint8_t i = 0; i < TestFixture::bufferSize; i++)
    {
        EXPECT_EQ(TestFixture::mRingBufferTestInterface.append(asSliceUnsafe(&i, 1)),
                  OperationResult::success);
    }

    // buffer should be full, but only half is continuous
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(),
              TestFixture::bufferSize);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableContinuousElements(),
              TestFixture::bufferSize / 2);

    // first half
    slice = TestFixture::mRingBufferTestInterface.peek(TestFixture::bufferSize);
    ASSERT_EQ(slice.getNumberOfElements(), TestFixture::bufferSize / 2);
    EXPECT_SLICE_EQ(slice, asSlice(std::array<uint8_t, 5>({0, 1, 2, 3, 4})));

    // second half
    slice = TestFixture::mRingBufferTestInterface.peek(TestFixture::bufferSize / 2,
                                                       TestFixture::bufferSize / 2);
    ASSERT_EQ(slice.getNumberOfElements(), TestFixture::bufferSize / 2);
    EXPECT_SLICE_EQ(slice, asSlice(std::array<uint8_t, 5>({5, 6, 7, 8, 9})));
}

TYPED_TEST(DirectAccessRingBufferTest, pop)
{
    TestFixture::fillWithTestData(TestFixture::bufferSize);
    const size_t chunkSize = 2;  // important to be divisible by TestFixture::bufferSize

    for (size_t i = 0; i < TestFixture::bufferSize / chunkSize; i++)
    {
        EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(),
                  TestFixture::bufferSize - (i * chunkSize));

        const auto slice = TestFixture::mRingBufferTestInterface.pop(chunkSize);
        EXPECT_EQ(slice.getNumberOfElements(), chunkSize);
        EXPECT_EQ(i, slice[0]);
        EXPECT_EQ(i + 10, slice[1]);
    }

    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty());
}

TYPED_TEST(DirectAccessRingBufferTest, popWithUnderflowAndMisaligned)
{
    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty());
    // pop on empty
    const auto slice = TestFixture::mRingBufferTestInterface.pop(5);
    EXPECT_EQ(slice.getNumberOfElements(), 0U);

    // fill half, request half, get half
    TestFixture::fillWithTestData(TestFixture::bufferSize / 2);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.pop(TestFixture::bufferSize / 2)
                      .getNumberOfElements(),
              TestFixture::bufferSize / 2);

    // pointer now at half, so two continuous chunks
    TestFixture::fillWithTestData(TestFixture::bufferSize);
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(),
              TestFixture::bufferSize);  // full
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableContinuousElements(),
              TestFixture::bufferSize / 2);  // half
    {
        // should get only half, because it wraps around at the end of the underlying buffer
        const auto res = TestFixture::mRingBufferTestInterface.pop(TestFixture::bufferSize);
        EXPECT_EQ(res.getNumberOfElements(), TestFixture::bufferSize / 2);
        // TODO: Check values
    }

    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableElements(),
              TestFixture::bufferSize / 2);  // half
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.getAvailableContinuousElements(),
              TestFixture::bufferSize / 2);  // half
    {
        // only one over boundary, and second half remaining
        const auto res =
                TestFixture::mRingBufferTestInterface.pop((TestFixture::bufferSize / 2) + 1);
        EXPECT_EQ(res.getNumberOfElements(), TestFixture::bufferSize / 2);
        // TODO: Check values
    }

    // should be empty now
    EXPECT_TRUE(TestFixture::mRingBufferTestInterface.isEmpty());

    EXPECT_EQ(TestFixture::mRingBufferTestInterface.pop(TestFixture::bufferSize / 2)
                      .getNumberOfElements(),
              0U);
}

TYPED_TEST(DirectAccessRingBufferTest, invalidPeekWouldUnderflow)
{
    uint8_t buf[TestFixture::bufferSize];

    // is empty
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.peekInto(asSlice(buf),
                                                             TestFixture::bufferSize + 1),
              OperationResult::readWriteTooBig);

    TestFixture::fillWithTestData(TestFixture::bufferSize);

    // contains something, but read is completely outside boundary
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.peekInto(asSlice(buf),
                                                             TestFixture::bufferSize + 1),
              OperationResult::readWriteTooBig);

    // contains something, but last element is outside boundary
    EXPECT_EQ(TestFixture::mRingBufferTestInterface.peekInto(asSlice(buf), 1),
              OperationResult::readWriteTooBig);
}
