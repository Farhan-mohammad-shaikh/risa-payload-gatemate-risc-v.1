/*
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

#include "outpost/container/memory_pool.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/harness.h>

using namespace outpost::container;
using namespace ::testing;

// ---------------------------------------------------------------------------
static constexpr auto blockSize = 100u;
static constexpr int anyValue = 42;

class Element
{
public:
    static bool destructorCalled;

    explicit Element() : mValue(anyValue)
    {
    }

    explicit Element(int value) : mValue(value)
    {
    }

    ~Element()
    {
        destructorCalled = true;
    }

    int mValue;
};

bool Element::destructorCalled = false;

// ---------------------------------------------------------------------------
template <typename Trait>
class MemoryPoolTest : public ::testing::Test
{
public:
    static constexpr auto numberOfElements = Trait::numberOfElements;
    MemoryPool<blockSize, numberOfElements> mPool;

    static_assert(
            std::is_same_v<typename Trait::ExpectedIndexType, typename decltype(mPool)::IndexType>,
            "Unexpected IndexType");
};

template <size_t N, typename IndexType>
struct TestTrait
{
    static constexpr size_t numberOfElements = N;
    using ExpectedIndexType = IndexType;
};

using TestTraits = ::testing::Types<TestTrait<255, uint8_t>,
                                    TestTrait<256, uint16_t> /*,
                                    TestTrait<65535, uint16_t>,
                                    TestTrait<65536, uint32_t> */>;

// False-positive cppcheck finding: This is not a syntax error!
// cppcheck-suppress syntaxError
TYPED_TEST_SUITE(MemoryPoolTest, TestTraits, );

// ---------------------------------------------------------------------------
TYPED_TEST(MemoryPoolTest, shouldBeAvailable)
{
    ASSERT_TRUE(TestFixture::mPool.isAvailable());
}

TYPED_TEST(MemoryPoolTest, shouldRaiseAssertionWhenFreeingInvalidPointer)
{
    uint8_t buffer[blockSize];
    uint8_t* notInBuffer = &buffer[0];
    (void) notInBuffer;

    auto elem = TestFixture::mPool.allocate();
    uint8_t* invalidStartAddr = &elem[1];
    (void) invalidStartAddr;

    EXPECT_OUTPOST_ASSERTION_DEATH(TestFixture::mPool.free(notInBuffer),
                                   "Chunk is not part of this pool");
    EXPECT_OUTPOST_ASSERTION_DEATH(TestFixture::mPool.free(invalidStartAddr),
                                   "Does not point to start of chunk");

    TestFixture::mPool.free(elem);
}

TYPED_TEST(MemoryPoolTest, shouldBeNotAvailable)
{
    std::vector<outpost::Slice<uint8_t>> elements;

    for (uint i = 0; i < TestFixture::numberOfElements; i++)
    {
        elements.emplace_back(TestFixture::mPool.allocate());
    }

    ASSERT_FALSE(TestFixture::mPool.isAvailable());

    auto notValid = TestFixture::mPool.allocate();

    ASSERT_THAT(notValid.begin(), IsNull());

    for (auto& element : elements)
    {
        TestFixture::mPool.free(element);
    }
}

TYPED_TEST(MemoryPoolTest, shouldRaiseOrIgnoreNullptr)
{
    uint8_t* ptrToNull = nullptr;

#if defined(OUTPOST_USE_ASSERT) && OUTPOST_USE_ASSERT == 1
    EXPECT_OUTPOST_ASSERTION_DEATH(TestFixture::mPool.free(outpost::Slice<uint8_t>::empty()),
                                   "Can not free nullptr");
    EXPECT_OUTPOST_ASSERTION_DEATH(TestFixture::mPool.free(ptrToNull), "Can not free nullptr");
#else
    // for coverage (nothing bad should happen):
    TestFixture::mPool.free(outpost::Slice<uint8_t>::empty());
    TestFixture::mPool.free(ptrToNull);
#endif
}

TYPED_TEST(MemoryPoolTest, shouldAllocBlock)
{
    auto mem = TestFixture::mPool.allocate();

    ASSERT_THAT(mem.begin(), Not(IsNull()));

    TestFixture::mPool.free(mem);
}

TYPED_TEST(MemoryPoolTest, shouldFreeBlock)
{
    auto mem = TestFixture::mPool.allocate();
    TestFixture::mPool.free(mem);

    ASSERT_THAT(TestFixture::mPool.getNumberOfFreeChunks(), Eq(TestFixture::numberOfElements));
}

TYPED_TEST(MemoryPoolTest, shouldFreeAndAllocBlock)
{
    std::vector<outpost::Slice<uint8_t>> elements;

    auto mem = TestFixture::mPool.allocate();
    for (uint i = 0; i < TestFixture::numberOfElements - 1; i++)
    {
        elements.emplace_back(TestFixture::mPool.allocate());
    }
    TestFixture::mPool.free(mem);
    mem = TestFixture::mPool.allocate();

    ASSERT_THAT(mem.begin(), Not(IsNull()));

    TestFixture::mPool.free(mem);

    for (auto& element : elements)
    {
        TestFixture::mPool.free(element);
    }

    ASSERT_THAT(TestFixture::mPool.getNumberOfFreeChunks(), TestFixture::numberOfElements);
}

TYPED_TEST(MemoryPoolTest, shouldFreeAllAllocatedBlocks)
{
    std::vector<outpost::Slice<uint8_t>> blocks;
    for (uint i = 0; i < TestFixture::numberOfElements; i++)
    {
        blocks.push_back(TestFixture::mPool.allocate());
    }

    for (auto& ptr : blocks)
    {
        TestFixture::mPool.free(ptr);
        EXPECT_EQ(ptr.getNumberOfElements(), 0U);
    }

    ASSERT_THAT(TestFixture::mPool.getNumberOfFreeChunks(), TestFixture::numberOfElements);
}

TYPED_TEST(MemoryPoolTest, shouldPerformSequenceOfAllocsAndFree)
{
    MemoryPool<5, TestFixture::numberOfElements> pool;

    auto ptr0 = TestFixture::mPool.allocate();
    auto ptr1 = TestFixture::mPool.allocate();
    auto ptr2 = TestFixture::mPool.allocate();
    auto ptr3 = TestFixture::mPool.allocate();
    auto ptr4 = TestFixture::mPool.allocate();

    TestFixture::mPool.free(ptr0);
    TestFixture::mPool.free(ptr4);

    ptr0 = TestFixture::mPool.allocate();
    ptr4 = TestFixture::mPool.allocate();

    TestFixture::mPool.free(ptr4);
    TestFixture::mPool.free(ptr3);
    TestFixture::mPool.free(ptr2);
    TestFixture::mPool.free(ptr1);
    TestFixture::mPool.free(ptr0);

    ptr0 = TestFixture::mPool.allocate();
    TestFixture::mPool.free(ptr0);

    ASSERT_THAT(TestFixture::mPool.getNumberOfFreeChunks(), TestFixture::numberOfElements);
}

// ---------------------------------------------------------------------------
TEST(ObjectPool, shouldAllocAndFreeObject)
{
    ObjectPool<Element, 1> pool;

    auto* e = pool.newObject(anyValue);
    int value = e->mValue;
    Element::destructorCalled = false;
    pool.deleteObject(e);

    ASSERT_EQ(value, anyValue);
    ASSERT_TRUE(Element::destructorCalled);
}

TEST(ObjectPool, shouldReturnNullptrWhenPoolIsEmpty)
{
    std::vector<Element*> objs;
    static constexpr size_t numberOfObjs = 10;
    ObjectPool<Element, numberOfObjs> pool;

    for (uint i = 0; i < numberOfObjs; i++)
    {
        objs.emplace_back(pool.newObject(anyValue));
    }

    auto* ret = pool.newObject(anyValue);

    ASSERT_THAT(ret, IsNull());

    for (auto* obj : objs)
    {
        pool.deleteObject(obj);
    }
}

TEST(ObjectPool, shouldIgnoreOrRaiseErrorWhenReturningANullptr)
{
    ObjectPool<Element, 1> pool;
    Element* elem = nullptr;

#if defined(OUTPOST_USE_ASSERT) && OUTPOST_USE_ASSERT == 1
    EXPECT_OUTPOST_ASSERTION_DEATH(pool.deleteObject(elem), "Can not delete nullptr");
#else
    // for coverage (nothing bad should happen):
    pool.deleteObject(elem);
#endif
}
