/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/base/bounded_string.h>

#include <unittest/bounded_string_helper.h>
#include <unittest/harness.h>

using namespace outpost;
using outpost::Slice;

template <size_t N>
size_t
size(const char (&)[N])
{
    return N;
}

TEST(BoundedStringTest, strnlenBoundaries)
{
    static_assert(BoundedString::strnlen("1234", 5) == 4, "under max number of elements");
    static_assert(BoundedString::strnlen("12345", 5) == 5, "max number of elements");
    static_assert(BoundedString::strnlen("123456", 5) == 5, "over max number of elements");
    static_assert(BoundedString::strnlen(nullptr, 5) == 0, "nullptr element");

    // and the same for dynamic checks (coverage...)
    char* str;
    std::array<char, 7> buf;
    memcpy(buf.begin(), "12346", 6);
    str = buf.begin();
    str[buf.size() - 1] = 0;
    EXPECT_EQ(BoundedString::strnlen(str, 5U), 5U) << "over max number of elements";
    str[buf.size() - 2] = 0;
    EXPECT_EQ(BoundedString::strnlen(str, 5U), 5U) << "max number of elements";
    str[buf.size() - 3] = 0;
    EXPECT_EQ(BoundedString::strnlen(str, 5U), 4U) << "under max number of elements";
    str = nullptr;
    EXPECT_EQ(BoundedString::strnlen(str, 5U), 0U) << "nullptr element";
}

TEST(BoundedStringTest, constructors)
{
    BoundedString empty;
    EXPECT_EQ(empty.size(), 0U);

    const char* staticString = "123456789";

    // simple default "from char *" (implicitly using std::basic_string_view)
    BoundedString justpointer{staticString};
    EXPECT_EQ(justpointer.size(), 9U);
    EXPECT_TRUE(SliceMatch(asSliceUnsafe(staticString, strlen(staticString)), justpointer));

    // From c-array
    {
        constexpr char notDegradedArray[] = "123456789";
        static_assert(BoundedString{notDegradedArray}.size() == 9, "Not constexpr");
        const auto fromArray = BoundedString{notDegradedArray};
        EXPECT_EQ(fromArray.size(), 9U);
        const BoundedString fromZeroArray = notDegradedArray;
        EXPECT_EQ(fromZeroArray.size(), 9U);
        EXPECT_EQ(justpointer, fromZeroArray);
    }

    // from std array
    {
        std::array<char, 10> array = {0};
        strncpy(array.begin(), staticString, array.size());

        const auto fromArray = BoundedString{array};
        EXPECT_EQ(fromArray.size(), 9U);
        EXPECT_EQ(justpointer, fromArray);

        // No zero termination, so expect a "full" array
        array.fill('A');
        const auto fromNoZeroTermination = BoundedString{array};
        EXPECT_EQ(fromNoZeroTermination.size(), array.size());  // Just hit max
    }

    // from dynamic C array (not preferred!)
    {
        char array[12];
        strncpy(array, staticString, size(array));

        const auto fromArray = BoundedString{array};
        EXPECT_EQ(fromArray.size(), 9U);
        EXPECT_EQ(justpointer, fromArray);

        // No zero termination, so expect a "full" array
        memset(array, 'A', 12);
        const auto fromNoZeroTermination = BoundedString{array};
        EXPECT_EQ(fromNoZeroTermination.size(), 12U);  // Just hit max
    }

    {
        using namespace std::literals;
        constexpr BoundedString fromView = "123456789"sv;
        static_assert(fromView.size() == 9, "Constexpr size did not work");
        EXPECT_EQ(fromView.size(), 9U);
        EXPECT_EQ(justpointer, fromView);
    }

    BoundedString byFirstLast(staticString, &staticString[9]);
    EXPECT_EQ(byFirstLast.size(), 9U);
    EXPECT_EQ(justpointer, byFirstLast);

    BoundedString moveConstructor{BoundedString{staticString}};
    EXPECT_EQ(moveConstructor.size(), 9U);
    EXPECT_EQ(justpointer, moveConstructor);

    BoundedString assign;
    assign = staticString;
    EXPECT_EQ(assign.size(), 9U);
    EXPECT_EQ(justpointer, assign);

    const auto sliceToString = asSliceUnsafe(staticString, strlen(staticString));
    // this will _not_ strlen itself or skip the last element, because it is from "trusted" Slice!
    BoundedString fromConstSlice = sliceToString;
    EXPECT_EQ(fromConstSlice.size(), 9U);
    EXPECT_EQ(justpointer, fromConstSlice);
}

inline void
passByReferenceEQ(const BoundedString& actual, const std::basic_string_view<char>& expected)
{
    EXPECT_EQ(actual, expected);
}

TEST(BoundedStringTest, passByReference)
{
    const char* pointer = "abc";
    passByReferenceEQ(pointer, "abc");

    char carray[] = "cde";
    passByReferenceEQ(carray, "cde");
}

TEST(BoundedStringTest, fromUint8Buffer)
{
    const BoundedString expected{"12345"};
    std::array<uint8_t, 10> buf;
    const auto maybeUint8buf = asSlice(buf).concatenateCopyFrom(
            {expected.asUint8Slice()});  // Note: no zero termination!
    ASSERT_TRUE(maybeUint8buf.has_value());

    const auto fromRange = BoundedString{&buf[0], &buf[5]};
    const auto fromSlice = BoundedString{*maybeUint8buf};
    EXPECT_EQ(expected, fromRange);
    EXPECT_EQ(expected, fromSlice);
}

TEST(BoundedStringTest, comparisonOperators)
{
    const BoundedString a = "1234";
    const BoundedString b = "12345";

    EXPECT_NE(a, b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == b);

    EXPECT_EQ(a, b.skipLast(1));
    EXPECT_FALSE(a != b.skipLast(1));
    EXPECT_TRUE(a == b.skipLast(1));

    EXPECT_EQ(b, "12345");
}

TEST(BoundedStringTest, forwardedSliceFunctions)
{
    const BoundedString a = "1234";

    EXPECT_EQ(a.skipFirst(1), "234");
    EXPECT_EQ(a.skipLast(1), "123");
    EXPECT_EQ(a.first(1), "1");
    EXPECT_EQ(a.last(1), "4");
    EXPECT_EQ(a.subRange(1, 3), "23");
}

TEST(BoundedStringTest, copyBack)
{
    BoundedString str = "Test1234";
    constexpr size_t testLen = 9;

    ASSERT_EQ(size("Test1234"), testLen);  // including null
    ASSERT_EQ(size("Test1234"), strlen("Test1234") + 1);
    ASSERT_EQ(size("Test1234"), str.size() + 1);

    std::array<char, testLen> exactBuf = {{0}};
    const Slice<char> zeroTerminated = str.copyZeroTerminatedInto(asSlice(exactBuf));

    EXPECT_TRUE(SliceMatch(zeroTerminated, asSlice(exactBuf)));
    EXPECT_ARRAY_EQ(char, "Test1234", exactBuf.begin(), exactBuf.size());
    EXPECT_EQ(strlen(zeroTerminated.getDataPointer()), 8U);

    const char* unsafe = str.copyZeroTerminatedIntoUnsafe(asSlice(exactBuf));
    ASSERT_NE(unsafe, nullptr);
    EXPECT_ARRAY_EQ(char, "Test1234", unsafe, 9);
    EXPECT_EQ(strlen(unsafe), 8U);
}

TEST(BoundedStringTest, copyBackEmpty)
{
    BoundedString str = "Test1234";
    ASSERT_EQ(ARRAY_SIZE("Test1234"), 9U);  // including null
    ASSERT_EQ(str.size(), 8U);              // not including null

    char oneTooSmallBuf[8] = {0};
    for (const auto b : oneTooSmallBuf)
    {
        EXPECT_EQ(b, 0);
    }

    const Slice<char> zeroTerminated = str.copyZeroTerminatedInto(asSlice(oneTooSmallBuf));
    EXPECT_EQ(zeroTerminated.getNumberOfElements(), 0U);
    EXPECT_OUTPOST_CONSTEXPR_ASSERTION_DEATH(zeroTerminated[0], "out of bounds");

    const char* unsafe = str.copyZeroTerminatedIntoUnsafe(asSlice(oneTooSmallBuf));
    ASSERT_EQ(unsafe, nullptr);
}

TEST(BoundedStringTest, coutWithStub)
{
    BoundedString str = "Test1234";
    testing::internal::CaptureStdout();
    std::cout << to_string(str);
    std::string coutput = testing::internal::GetCapturedStdout();
    EXPECT_EQ(coutput, "Test1234");
}

TEST(BoundedStringTest, findLastOf)
{
    const BoundedString empty;
    EXPECT_FALSE(empty.findLastOf('/'));

    const BoundedString hasNot = "abc";
    EXPECT_FALSE(hasNot.findLastOf('/'));

    const BoundedString has = "a/b/c";
    const auto maybeLastSlash = has.findLastOf('/');
    ASSERT_TRUE(maybeLastSlash);
    EXPECT_EQ(*maybeLastSlash, 3U);

    const BoundedString sub1 = has.first(*maybeLastSlash);
    EXPECT_EQ(sub1, "a/b");

    const auto maybeSecondLastSlash = sub1.findLastOf('/');
    ASSERT_TRUE(maybeSecondLastSlash);
    EXPECT_EQ(*maybeSecondLastSlash, 1U);

    const BoundedString sub2 = sub1.first(*maybeSecondLastSlash);
    EXPECT_EQ(sub2, "a");

    const auto maybeThirdLastSlash = sub2.findFirstOf('/');
    ASSERT_FALSE(maybeThirdLastSlash);
}

TEST(BoundedStringTest, findFirstOf)
{
    const BoundedString empty;
    EXPECT_FALSE(empty.findFirstOf('/'));

    const BoundedString hasNot = "abc";
    EXPECT_FALSE(hasNot.findFirstOf('/'));

    const BoundedString has = "a/b/c";
    const auto maybeFirstSlash = has.findFirstOf('/');
    ASSERT_TRUE(maybeFirstSlash);
    EXPECT_EQ(*maybeFirstSlash, 1U);

    const BoundedString sub1 = has.skipFirst(*maybeFirstSlash + 1);
    EXPECT_EQ(sub1, "b/c");

    const auto maybeSecondSlash = sub1.findFirstOf('/');
    ASSERT_TRUE(maybeSecondSlash);
    EXPECT_EQ(*maybeSecondSlash, 1U);

    const BoundedString sub2 = sub1.skipFirst(*maybeSecondSlash + 1);
    EXPECT_EQ(sub2, "c");

    const auto maybeThirdSlash = sub2.findFirstOf('/');
    ASSERT_FALSE(maybeThirdSlash);
}
