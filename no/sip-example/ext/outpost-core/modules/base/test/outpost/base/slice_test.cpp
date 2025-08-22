/*
 * Copyright (c) 2014-2018, Fabian Greif
 * Copyright (c) 2019-2021, Jan Malburg
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Sommer, Jan
 * Copyright (c) 2023-2024, Jan-Gerd Mess
 * Copyright (c) 2023-2024, Pieper, Pascal
 * Copyright (c) 2023-2024, Janosch Reinking
 * Copyright (c) 2023-2024, Pascal Pieper
 * Copyright (c) 2024, Passenberg, Felix Constantin
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/base/slice.h>

#include <unittest/base/slice.h>
#include <unittest/harness.h>

#include <array>
#include <numeric>

using outpost::asSlice;
using outpost::Slice;

using unittest::base::SliceEq;
using unittest::base::SliceSizeIs;
using namespace ::testing;

// ---------------------------------------------------------------------------
TEST(SliceTest, sliceEq)
{
    std::array<uint8_t, 1> buffer1 = {1};
    std::array<uint8_t, 1> buffer2 = {2};
    std::array<uint8_t, 2> buffer3 = {1, 2};

    auto a = outpost::asSlice(buffer1);
    auto b = outpost::asSlice(buffer2);
    auto c = outpost::asSlice(buffer3);

    EXPECT_THAT(a, SliceEq(a));
    // EXPECT_THAT(a, SliceEq(b));
    // EXPECT_THAT(a, SliceEq(c));

    // EXPECT_THAT(a, ::testing::Not(SliceEq(a)));
    EXPECT_THAT(a, Not(SliceEq(b)));
    EXPECT_THAT(a, Not(SliceEq(c)));
}

TEST(SliceTest, sliceSizeIs)
{
    std::array<uint8_t, 1> buffer1 = {1};
    auto a = outpost::asSlice(buffer1);

    EXPECT_THAT(a, SliceSizeIs(Eq(1U)));
    // EXPECT_THAT(a, SliceSizeIs(Eq(2U)));

    EXPECT_THAT(a, SliceSizeIs(Lt(2U)));
    // EXPECT_THAT(a, SliceSizeIs(Gt(2U)));

    // EXPECT_THAT(a, SliceSizeIs(Not(Lt(2U))));
    EXPECT_THAT(a, SliceSizeIs(Not(Gt(2U))));
}

// ---------------------------------------------------------------------------
TEST(SliceTest, emptySlice)
{
    std::array<uint8_t, 1> buffer;

    auto slice = outpost::asSlice(buffer);
    EXPECT_THAT(slice, Not(SliceEq(outpost::Slice<uint8_t>::empty())));

    slice = outpost::emptySlice();
    EXPECT_THAT(slice, SliceEq(outpost::Slice<uint8_t>::empty()));
}

TEST(SliceTest, zeroSize)
{
    std::array<uint32_t, 0> data;

    // this should not cause compiler errors
    Slice<uint32_t> slice = asSlice(data);
    EXPECT_EQ(slice.getNumberOfElements(), 0U);
    EXPECT_EQ(slice.begin(), slice.end());
#if defined(OUTPOST_USE_ASSERT) && OUTPOST_USE_ASSERT == 1 && __cplusplus >= 201402L
    EXPECT_DEATH(slice.getDataPointer(), "Empty Slice data pointer access");
#else
    // special test: Check default behavior in unasserted build
    EXPECT_EQ(nullptr, slice.getDataPointer());
#endif
    Slice<uint32_t> slice2(data);
    EXPECT_EQ(slice2.getNumberOfElements(), 0U);
    EXPECT_EQ(slice2.begin(), slice2.end());
}

TEST(SliceTest, assignmentOperators)
{
    std::array<uint32_t, 2> data;

    auto rvalue = Slice<uint32_t>::empty();
    // cppcheck-suppress redundantInitialization
    rvalue = asSlice(data);
    EXPECT_EQ(rvalue.getNumberOfElements(), 2U);

    auto lvalue = Slice<uint32_t>::empty();
    // cppcheck-suppress redundantInitialization
    lvalue = rvalue;
    EXPECT_EQ(lvalue.getNumberOfElements(), 2U);
}

TEST(SliceTest, toUint8Slice)
{
    uint32_t data[4] = {0};

    Slice<uint32_t> array(data);
    Slice<uint8_t> array2 = array.asUint8Slice();
    for (size_t i = 0; i < array2.getNumberOfElements(); i++)
    {
        array2[i] = 0xff;
    }

    EXPECT_EQ(0xffffffff, array[0]);
    EXPECT_EQ(0xffffffff, array[1]);
    EXPECT_EQ(0xffffffff, array[2]);
    EXPECT_EQ(0xffffffff, array[3]);

    using const_slice_32 = const Slice<uint32_t>;
    EXPECT_EQ(false, std::is_const<Slice<uint32_t>::uint8Type>::value);
    EXPECT_EQ(true, std::is_const<Slice<const uint32_t>::uint8Type>::value);
    EXPECT_EQ(false, std::is_const<const_slice_32::uint8Type>::value);
}

TEST(SliceTest, fill)
{
    uint32_t data[4] = {0};

    Slice<uint32_t> slice(data);
    slice.fill(0x1f2e3d4cu);

    EXPECT_EQ(0x1f2e3d4cu, slice[0]);
    EXPECT_EQ(0x1f2e3d4cu, slice[1]);
    EXPECT_EQ(0x1f2e3d4cu, slice[2]);
    EXPECT_EQ(0x1f2e3d4cu, slice[3]);

    // check it does not prevent const type slices
    const uint32_t data2[4] = {0};
    // cppcheck-suppress unreadVariable
    Slice<const uint32_t> slice2(data2);
}

TEST(SliceTest, createFromCStyleArray)
{
    uint8_t data[7];

    Slice<uint8_t> array(data);

    EXPECT_EQ(7U, array.getNumberOfElements());
}

TEST(SliceTest, createFromSTLCompatibleContainer)
{
    std::vector<int> data{0, 1};

    auto array = outpost::asSlice(data);

    EXPECT_EQ(2U, array.getNumberOfElements());
}

TEST(SliceTest, dataAccessWithCStyleArray)
{
    uint8_t data[6] = {6, 5, 4, 3, 2, 1};

    Slice<uint8_t> array(data);

    EXPECT_EQ(6U, array.getNumberOfElements());
    EXPECT_EQ(6, array[0]);
    EXPECT_EQ(5, array[1]);
    EXPECT_EQ(2, array[4]);
}

TEST(SliceTest, createWithExplicitSize)
{
    uint8_t data[6];

    {
        auto array = Slice<uint8_t>::unsafe(data, 4);
        EXPECT_EQ(4U, array.getNumberOfElements());
    }

    {
        auto array = outpost::asSliceUnsafe(data, 4);
        EXPECT_EQ(4U, array.getNumberOfElements());
    }
}

TEST(SliceTest, createArrayOfPointers)
{
    class TestClass
    {
    };

    TestClass testClass1;
    TestClass testClass2;
    TestClass testClass3;

    TestClass* data[3] = {&testClass1, &testClass2, &testClass3};

    Slice<TestClass*> array(data);

    EXPECT_EQ(3U, array.getNumberOfElements());

    EXPECT_EQ(&testClass1, array[0]);
    EXPECT_EQ(&testClass2, array[1]);
}

TEST(SliceTest, shouldBeConstructableFromStdArray)
{
    std::array<uint8_t, 10> array;
    auto slice = Slice<uint8_t>(array);
    EXPECT_EQ(array.size(), slice.getNumberOfElements());
}

TEST(SliceTest, asSliceFromStdArray)
{
    std::array<uint8_t, 10> array;
    auto slice = asSlice(array);
    EXPECT_EQ(array.size(), slice.getNumberOfElements());

    const std::array<uint8_t, 10> const_array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto slice2 = asSlice<uint8_t>(const_array);
    EXPECT_EQ(const_array.size(), slice2.getNumberOfElements());
}

TEST(SliceTest, shouldBeConstructableFromStdVector)
{
    std::vector<uint8_t> vector(5);
    auto slice = Slice<uint8_t>(vector);
    EXPECT_EQ(vector.size(), slice.getNumberOfElements());
}

TEST(SliceTest, shouldCreateSubSlicesFromBeginning)
{
    std::array<uint8_t, 10> array = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    auto slice = asSlice(array);

    auto subslice = slice.first(3);

    ASSERT_EQ(3U, subslice.getNumberOfElements());

    EXPECT_EQ(0U, subslice[0]);
    EXPECT_EQ(1U, subslice[1]);
    EXPECT_EQ(2U, subslice[2]);
}

TEST(SliceTest, shouldCreateSubSlicesFromEnd)
{
    std::array<uint8_t, 10> array = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    auto slice = asSlice(array);

    auto subslice = slice.last(3);

    ASSERT_EQ(3U, subslice.getNumberOfElements());

    EXPECT_EQ(7U, subslice[0]);
    EXPECT_EQ(8U, subslice[1]);
    EXPECT_EQ(9U, subslice[2]);
}

TEST(SliceTest, shouldCreateNestedSubSlices)
{
    std::array<uint8_t, 10> array = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    auto slice = asSlice(array);

    auto subslice = slice.first(6).last(3);

    ASSERT_EQ(3U, subslice.getNumberOfElements());

    EXPECT_EQ(3U, subslice[0]);
    EXPECT_EQ(4U, subslice[1]);
    EXPECT_EQ(5U, subslice[2]);
}

TEST(SliceTest, shouldBeCompatibleWithGslSpan)
{
    std::array<uint8_t, 10> array;
    auto span = gsl::make_span(array);

    // Create from gsl::span
    auto slice = outpost::asSlice(span);
    EXPECT_EQ(array.size(), slice.getNumberOfElements());

    // Convert back to gsl::span
    auto span2 = slice.asSpan();
    EXPECT_EQ(array.size(), static_cast<std::size_t>(span2.size()));
}

TEST(SliceTest, shouldConvertToConstSlice)
{
    std::array<uint8_t, 10> array;
    outpost::Slice<uint8_t> slice(array);

    outpost::Slice<const uint8_t> constSlice(slice);

    EXPECT_EQ(array.size(), constSlice.getNumberOfElements());
}

TEST(SliceTest, shouldProvideIteratorInterface)
{
    std::array<uint8_t, 4> array = {{1, 4, 5, 7}};
    outpost::Slice<uint8_t> slice(array);

    size_t index = 0;
    for (uint8_t& data : slice)
    {
        EXPECT_EQ(array[index], data);
        data = 3;
        index++;
    }

    // check that the data could be manipulated
    for (uint8_t& data : slice)
    {
        EXPECT_EQ(3, data);
    }

    EXPECT_EQ(array.size(), index);
}

TEST(SliceTest, shouldProvideConstIteratorInterface)
{
    std::array<uint8_t, 4> array = {{1, 4, 5, 7}};
    outpost::Slice<const uint8_t> slice(array);

    static_assert(std::is_same<outpost::Slice<const uint8_t>::iterator,
                               outpost::Slice<const uint8_t>::const_iterator>::value,
                  "iterator over Slice<const T> must be const_iterator");
    static_assert(
            std::is_same<const uint8_t*, outpost::Slice<const uint8_t>::const_iterator>::value,
            "const_iterator over Slice<const T> is of 'const T*'");
    static_assert(std::is_same<const uint8_t*, outpost::Slice<const uint8_t>::iterator>::value,
                  "iterator over Slice<const T> is of 'const T*'");
    static_assert(std::is_same<const uint8_t*, outpost::Slice<uint8_t>::const_iterator>::value,
                  "const_iterator over Slice<T> is of 'const T*'");

    size_t index = 0;
    for (uint8_t const& data : slice)
    {
        EXPECT_EQ(array[index], data);
        index++;
    }
    EXPECT_EQ(array.size(), index);
    EXPECT_THAT(array, testing::ElementsAreArray(slice.begin(), slice.end()));
    EXPECT_THAT(slice,
                testing::ElementsAreArray(array.begin(),
                                          array.end()));  // accesses slice::const_iterator

    outpost::Slice<const uint8_t>::iterator it_c = slice.begin();
    it_c++;
    // it_c = 9; // MUST also fails as the slice is over const values
    (void) it_c;  // use it_c

    index = 0;
    for (auto it = slice.begin(); it != slice.end(); it++)
    {
        EXPECT_EQ(array[index], *it);
        index++;
    }
}

TEST(SliceTest, constSliceDoesNotProtectData)
{
    std::array<uint8_t, 4> array = {{1, 4, 5, 7}};
    const outpost::Slice<uint8_t> slice(array);

    size_t index = 0;
    for (auto& data : slice)
    {
        EXPECT_EQ(array[index], data);
        data = 42;  // Iterator MUST NOT be const
        index++;
    }
    for (auto& data : slice)
    {
        EXPECT_EQ(42, data);
    }

    EXPECT_EQ(array.size(), index);
    index = 0;
    for (auto it = slice.begin(); it != slice.end(); it++)
    {
        EXPECT_EQ(array[index], *it);
        *it = 9;  // Iterator MUST NOT be const
        index++;
    }
    for (auto it = slice.begin(); it != slice.end(); it++)
    {
        EXPECT_EQ(9, *it);
    }

    EXPECT_EQ(array.size(), index);
    index = 0;
    for (index = 0; index < static_cast<size_t>(std::distance(slice.begin(), slice.end())); index++)
    {
        EXPECT_EQ(array[index], slice[index]);
        slice[index] = 9;  // operator[] MUST NOT be const
    }
    for (auto it = slice.begin(); it != slice.end(); it++)
    {
        EXPECT_EQ(9, *it);
    }

    outpost::Slice<uint8_t>::const_iterator c_it = slice.begin();
    c_it++;
    //*c_it = 9; // MUST fail as its a const iterator
    (void) c_it;  // use c_it
}

TEST(SliceTest, shouldProvideReverseIterator)
{
    std::array<uint8_t, 4> array = {{1, 4, 5, 7}};
    outpost::Slice<uint8_t> slice(array);

    size_t index = 0;
    for (auto it = slice.rbegin(); it != slice.rend(); ++it)
    {
        EXPECT_EQ(array[array.size() - index - 1], *it);
        index++;
    }
    EXPECT_EQ(array.size(), index);
}

TEST(SliceTest, shouldCreateSliceFromIteratorPair)
{
    std::array<uint8_t, 4> array = {{1, 4, 5, 7}};

    auto slice = outpost::Slice<uint8_t>(array.begin(), array.end());

    EXPECT_EQ(array.size(), slice.getNumberOfElements());
}

TEST(SliceTest, shouldCreateSubSliceFromIndexAndLength)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    auto slice = asSlice(array);

    auto slice1 = slice.subSlice(1, 2);
    EXPECT_EQ(2U, slice1.getNumberOfElements());
    EXPECT_EQ(2U, slice1[0]);
    EXPECT_EQ(3U, slice1[1]);
}

TEST(SliceTest, shouldCreateSubSliceFromTwoIndicies)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    auto slice = asSlice(array);

    auto slice1 = slice.subRange(1, 3);
    EXPECT_EQ(2U, slice1.getNumberOfElements());
    EXPECT_EQ(2U, slice1[0]);
    EXPECT_EQ(3U, slice1[1]);
}

TEST(SliceTest, shouldFirstLimitToAvailableElements)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    const auto slice = asSlice(array);

    const auto less = slice.first(2);
    EXPECT_EQ(less.getNumberOfElements(), 2U);

    const auto equal = slice.first(slice.getNumberOfElements());
    EXPECT_EQ(equal.getNumberOfElements(), slice.getNumberOfElements());

    const auto more = slice.first(slice.getNumberOfElements() + 2);
    EXPECT_EQ(more.getNumberOfElements(), slice.getNumberOfElements());
}

TEST(SliceTest, shouldLastLimitToAvailableElements)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    const auto slice = asSlice(array);

    const auto less = slice.last(2);
    EXPECT_EQ(less.getNumberOfElements(), 2U);

    const auto equal = slice.last(slice.getNumberOfElements());
    EXPECT_EQ(equal.getNumberOfElements(), slice.getNumberOfElements());

    const auto more = slice.last(slice.getNumberOfElements() + 2);
    EXPECT_EQ(more.getNumberOfElements(), slice.getNumberOfElements());
}

TEST(SliceTest, shouldCreateSubSliceAfterStartElements)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    auto slice = asSlice(array);

    auto slice1 = slice.skipFirst(2);
    EXPECT_EQ(2U, slice1.getNumberOfElements());
    EXPECT_EQ(3U, slice1[0]);
    EXPECT_EQ(4U, slice1[1]);
}

TEST(SliceTest, shouldCreateSkipNoneFromStart)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    auto slice = asSlice(array);

    auto slice1 = slice.skipFirst(0);
    EXPECT_EQ(4U, slice1.getNumberOfElements());
    EXPECT_EQ(1U, slice1[0]);
    EXPECT_EQ(4U, slice1[3]);
}

TEST(SliceTest, shouldCreateEmptySliceAfterSkippingTooManyElements)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    auto slice = asSlice(array);

    auto slice1 = slice.skipFirst(4);
    EXPECT_EQ(0U, slice1.getNumberOfElements());
}

TEST(SliceTest, shouldCreateSubSliceAfterEndElements)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    auto slice = asSlice(array);

    auto slice1 = slice.skipLast(2);
    EXPECT_EQ(2U, slice1.getNumberOfElements());
    EXPECT_EQ(1U, slice1[0]);
    EXPECT_EQ(2U, slice1[1]);
}

TEST(SliceTest, shouldCreateSkipNoneFromEnd)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    auto slice = asSlice(array);

    auto slice1 = slice.skipLast(0);
    EXPECT_EQ(4U, slice1.getNumberOfElements());
    EXPECT_EQ(1U, slice1[0]);
    EXPECT_EQ(4U, slice1[3]);
}

TEST(SliceTest, shouldCreateEmptySliceAfterSkippingTooManyElementsFromEnd)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};
    auto slice = asSlice(array);

    auto slice1 = slice.skipLast(4);
    EXPECT_EQ(0U, slice1.getNumberOfElements());
}

TEST(SliceTest, shouldBeUseableInConstexprContextForCArray)
{
    uint8_t array[4] = {1, 2, 3, 4};

    static_assert(asSlice(array).getNumberOfElements() == 4, "Must have length 4.");
}

TEST(SliceTest, shouldBeUseableInConstexprContextForStdArray)
{
    std::array<uint8_t, 4> array = {{1, 2, 3, 4}};

    static_assert(asSlice(array).getNumberOfElements() == 4, "Must have length 4.");
}

TEST(SliceTest, copyFromOfData)
{
    std::array<uint8_t, 4> array = {{}};
    auto slice = asSlice(array);
    ASSERT_EQ(0U, slice[0]);
    ASSERT_EQ(0U, slice[1]);
    ASSERT_EQ(0U, slice[2]);
    ASSERT_EQ(0U, slice[3]);

    std::array<uint8_t, 4> source = {{1, 2, 3, 4}};
    EXPECT_TRUE(slice.copyFrom(source.data(), source.size()));
    EXPECT_EQ(1U, slice[0]);
    EXPECT_EQ(2U, slice[1]);
    EXPECT_EQ(3U, slice[2]);
    EXPECT_EQ(4U, slice[3]);

    EXPECT_EQ(&slice[0], &array[0]);
    EXPECT_NE(&slice[0], &source[0]);
}

TEST(SliceTest, copyFromOfDataFails)
{
    std::array<uint8_t, 3> array = {{}};
    auto slice = asSlice(array);
    ASSERT_EQ(0U, slice[0]);
    ASSERT_EQ(0U, slice[1]);
    ASSERT_EQ(0U, slice[2]);

    std::array<uint8_t, 4> source = {{1, 2, 3, 4}};
    EXPECT_FALSE(slice.copyFrom(source.data(), source.size()));
    EXPECT_EQ(0U, slice[0]);
    EXPECT_EQ(0U, slice[1]);
    EXPECT_EQ(0U, slice[2]);
}

TEST(SliceTest, copyFromHandleZeroInputs)
{
    std::array<uint8_t, 3> array = {{}};
    auto slice = asSlice(array);
    ASSERT_EQ(0U, slice[0]);
    ASSERT_EQ(0U, slice[1]);
    ASSERT_EQ(0U, slice[2]);

    std::array<uint8_t, 4> source = {{1, 2, 3, 4}};
    // ok, zero-length copy is allowed
    EXPECT_TRUE(slice.copyFrom(source.data(), 0));
    // ok, because zero-length copy
    EXPECT_TRUE(slice.copyFrom(nullptr, 0));
    // not ok, pointer is invalid
    EXPECT_FALSE(slice.copyFrom(nullptr, 3));

    // neither of these calls should have changed anything
    EXPECT_EQ(0U, slice[0]);
    EXPECT_EQ(0U, slice[1]);
    EXPECT_EQ(0U, slice[2]);
}

TEST(SliceTest, copyFromOfSlices)
{
    std::array<uint8_t, 3> array = {{}};
    auto slice = asSlice(array);
    ASSERT_EQ(0U, slice[0]);
    ASSERT_EQ(0U, slice[1]);
    ASSERT_EQ(0U, slice[2]);

    // ok, fits
    std::array<uint8_t, 3> source1 = {{1, 2, 3}};
    auto slice1 = asSlice(source1);
    EXPECT_TRUE(slice.copyFrom(slice1));

    EXPECT_EQ(1U, slice[0]);
    EXPECT_EQ(2U, slice[1]);
    EXPECT_EQ(3U, slice[2]);

    // not ok, too large
    std::array<uint8_t, 4> source2 = {{4, 5, 6, 7}};
    auto slice2 = asSlice(source2);
    EXPECT_FALSE(slice.copyFrom(slice2));

    EXPECT_EQ(1U, slice[0]);
    EXPECT_EQ(2U, slice[1]);
    EXPECT_EQ(3U, slice[2]);

    // ok, nullptr protection
    auto slice3 = outpost::Slice<uint8_t>::empty();
    EXPECT_TRUE(slice.copyFrom(slice3));

    EXPECT_EQ(1U, slice[0]);
    EXPECT_EQ(2U, slice[1]);
    EXPECT_EQ(3U, slice[2]);
}

TEST(SliceTest, copyFromOfSlicesLargeType)
{
    std::array<uint64_t, 3> array = {{}};
    auto slice = asSlice(array);
    ASSERT_EQ(0U, slice[0]);
    ASSERT_EQ(0U, slice[1]);
    ASSERT_EQ(0U, slice[2]);

    // ok, fits
    std::array<uint64_t, 3> source1 = {{1, 2, 3}};
    auto slice1 = asSlice(source1);
    EXPECT_TRUE(slice.copyFrom(slice1));

    EXPECT_EQ(1U, slice[0]);
    EXPECT_EQ(2U, slice[1]);
    EXPECT_EQ(3U, slice[2]);

    // not ok, too large
    std::array<uint64_t, 4> source2 = {{4, 5, 6, 7}};
    auto slice2 = asSlice(source2);
    EXPECT_FALSE(slice.copyFrom(slice2));

    EXPECT_EQ(1U, slice[0]);
    EXPECT_EQ(2U, slice[1]);
    EXPECT_EQ(3U, slice[2]);

    // ok, nullptr protection
    auto slice3 = outpost::Slice<uint64_t>::empty();
    EXPECT_TRUE(slice.copyFrom(slice3));

    EXPECT_EQ(1U, slice[0]);
    EXPECT_EQ(2U, slice[1]);
    EXPECT_EQ(3U, slice[2]);
}

TEST(SliceTest, concatenateCopy)
{
    std::array<uint64_t, 10> targetArray = {{0}};
    auto targetSlice = asSlice(targetArray);

    std::array<const uint64_t, 3> source1Buf = {{1, 2, 3}};
    const auto source1 = asSlice(source1Buf);

    std::array<const uint64_t, 2> source2Buf = {{4, 5}};
    const auto source2 = asSlice(source2Buf);

    std::array<const uint64_t, 6> source3Buf = {{6, 7, 8, 9, 10, 11}};
    const auto source3 = asSlice(source3Buf);

    for (const auto& b : targetArray)
    {
        EXPECT_EQ(b, 0U) << "Initial values not zero";
    }

    // only one element
    {
        const auto maybeSubslice = targetSlice.concatenateCopyFrom({source1});
        ASSERT_TRUE(maybeSubslice);
        const auto subSlice = *maybeSubslice;
        EXPECT_EQ(subSlice.getNumberOfElements(), source1.getNumberOfElements());

        for (unsigned i = 0; i < targetSlice.getNumberOfElements(); i++)
        {
            if (i < source1.getNumberOfElements())
            {
                EXPECT_EQ(targetSlice[i], source1[i]) << "copied values in original slice do not "
                                                         "match";
                EXPECT_EQ(subSlice[i], source1[i]) << "copied values in view do not match";
            }
            else
            {
                EXPECT_EQ(targetSlice[i], 0U) << "copied over boundaries?";
            }
        }
    }

    // re-set to zero
    std::fill(targetArray.begin(), targetArray.end(), 0);
    for (const auto& b : targetArray)
    {
        EXPECT_EQ(b, 0U) << "Initial values not zero";
    }

    // two consecutive elements
    {
        const auto maybeSubslice = targetSlice.concatenateCopyFrom({source1, source2});
        ASSERT_TRUE(maybeSubslice);
        const auto subSlice = *maybeSubslice;
        EXPECT_EQ(subSlice.getNumberOfElements(),
                  source1.getNumberOfElements() + source2.getNumberOfElements());

        for (unsigned i = 0; i < targetSlice.getNumberOfElements(); i++)
        {
            if (i < source1.getNumberOfElements())
            {
                EXPECT_EQ(targetSlice[i], source1[i]) << "copied values slice 1 do not match";
                EXPECT_EQ(subSlice[i], source1[i]) << "copied values in view do not match";
            }
            else if (i < source1.getNumberOfElements() + source2.getNumberOfElements())
            {
                EXPECT_EQ(targetSlice[i], source2[i - source1.getNumberOfElements()])
                        << "copied values slice 2 do not match";
                EXPECT_EQ(subSlice[i], source2[i - source1.getNumberOfElements()])
                        << "copied values in view do not match";
            }
            else
            {
                EXPECT_EQ(targetSlice[i], 0U) << "copied over boundaries?";
            }
        }
    }

    // re-set to zero
    std::fill(targetArray.begin(), targetArray.end(), 0);
    for (const auto& b : targetArray)
    {
        EXPECT_EQ(b, 0U) << "Initial values not zero";
    }

    // three consecutive elements with overflow
    EXPECT_FALSE(targetSlice.concatenateCopyFrom({source1, source2, source3}));

    for (unsigned i = 0; i < targetSlice.getNumberOfElements(); i++)
    {
        if (i < source1.getNumberOfElements())
        {
            EXPECT_EQ(targetSlice[i], source1[i]) << "copied values slice 1 do not match";
        }
        else if (i < source1.getNumberOfElements() + source2.getNumberOfElements())
        {
            EXPECT_EQ(targetSlice[i], source2[i - source1.getNumberOfElements()])
                    << "copied values slice 2 do not match";
        }
        else
        {
            EXPECT_EQ(targetSlice[i], 0U) << "copied over boundaries?";
        }
    }
}

TEST(SliceTest, comparisonWithMemcmp)
{
    std::array<double, 64> a, b;
    static_assert(!outpost::has_equals_operator_v<double>,
                  "Test intended for types without explicit equal operator");

    std::iota(a.begin(), a.end(), 0);
    std::iota(b.begin(), b.end(), 0);

    const auto empty1 = Slice<const double>::empty();
    const auto empty2 = Slice<double>::empty();

    EXPECT_TRUE(empty1.isContentEqual(empty1));
    EXPECT_TRUE(empty1.isContentEqual(empty2));

    EXPECT_FALSE(empty1.isContentEqual(asSlice(a)));
    EXPECT_FALSE(asSlice(a).isContentEqual(empty1));

    EXPECT_TRUE(asSlice(a).isContentEqual(asSlice(a)));
    EXPECT_TRUE(asSlice(a).isContentEqual(asSlice(b)));
    EXPECT_TRUE(asSlice(b).isContentEqual(asSlice(a)));

    EXPECT_FALSE(asSlice(a).skipFirst(1).isContentEqual(asSlice(a)));
    EXPECT_FALSE(asSlice(a).isContentEqual(asSlice(a).skipFirst(1)));

    a[3] = 0;

    EXPECT_FALSE(asSlice(a).isContentEqual(asSlice(b)));
    EXPECT_FALSE(asSlice(b).isContentEqual(asSlice(a)));
}

TEST(SliceTest, comparisonWithEqualsOperator)
{
    struct WithOperator
    {
        double a;
        char b;

        constexpr void
        operator=(const int& n)
        {
            a = n / 2.;
            b = n;
        }

        constexpr bool
        operator==(const WithOperator& o) const
        {
            return (a == o.a && b == o.b);
        }
    };

    std::array<WithOperator, 64> a, b;
    static_assert(outpost::has_equals_operator_v<WithOperator>,
                  "Test intended for types with overloaded equals operator");

    std::iota(a.begin(), a.end(), 0);
    std::iota(b.begin(), b.end(), 0);

    const auto empty1 = Slice<const WithOperator>::empty();
    const auto empty2 = Slice<WithOperator>::empty();

    EXPECT_TRUE(empty1.isContentEqual(empty1));
    EXPECT_TRUE(empty1.isContentEqual(empty2));

    EXPECT_FALSE(empty1.isContentEqual(asSlice(a)));
    EXPECT_FALSE(asSlice(a).isContentEqual(empty1));

    EXPECT_TRUE(asSlice(a).isContentEqual(asSlice(a)));
    EXPECT_TRUE(asSlice(a).isContentEqual(asSlice(b)));
    EXPECT_TRUE(asSlice(b).isContentEqual(asSlice(a)));

    EXPECT_FALSE(asSlice(a).skipFirst(1).isContentEqual(asSlice(a)));
    EXPECT_FALSE(asSlice(a).isContentEqual(asSlice(a).skipFirst(1)));

    a[3] = 0;

    EXPECT_FALSE(asSlice(a).isContentEqual(asSlice(b)));
    EXPECT_FALSE(asSlice(b).isContentEqual(asSlice(a)));
}

TEST(SliceTest, expectDeathOnOutOfBoundsAccess)
{
    uint8_t buff[4] = {1, 2, 3, 4};
    const auto twoElementSlice = asSlice(buff).first(2);
    EXPECT_EQ(twoElementSlice[1], 2);
    EXPECT_OUTPOST_CONSTEXPR_ASSERTION_DEATH(twoElementSlice[2], "out of bounds");
    // first index bigger than second, that's an error
    EXPECT_OUTPOST_CONSTEXPR_ASSERTION_DEATH(twoElementSlice.subRange(2, 1),
                                             "requested invalid .* range");
}
