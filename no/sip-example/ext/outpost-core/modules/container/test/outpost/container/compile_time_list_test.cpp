/*
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

#include <outpost/container/compile_time_list.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <variant>

using namespace outpost;
using namespace outpost::container;
using namespace outpost::container::compile_time_list;

struct A
{
    int member1;
};

struct B
{
    int member1;
    int member2;
};

// ----------------------------------------------------------------------------

TEST(CompileTimeList, canBeInstatiatedEmpty)
{
    static constexpr CompileTimeList list{};
    static_assert(list.size() == 0, "Empty list should not have elements");

    static constexpr bool isEndActualEndType = std::is_same_v<decltype(list), const EmptyList>;
    static_assert(isEndActualEndType, "Empty list should be an empty list");
}

TEST(CompileTimeList, canBeInstantiated)
{
    static constexpr CompileTimeList list{A{1}, A{2}, B{3, 4}};
    static_assert(list.size() == 3, "list shoud have three members");
}

TEST(CompileTimeList, constructors)
{
    A a1{1};
    A a2{2};
    CompileTimeList list{std::move(a1), std::move(a2)};

    EXPECT_EQ(list.get().member1, 1);
    EXPECT_EQ(list.next().get().member1, 2);

    const CompileTimeList listCopy{list};

    EXPECT_EQ(listCopy.get().member1, 1);
    EXPECT_EQ(listCopy.next().get().member1, 2);

    const CompileTimeList listMove{std::move(list)};

    EXPECT_EQ(listMove.get().member1, 1);
    EXPECT_EQ(listMove.next().get().member1, 2);
}

TEST(CompileTimeList, accessElements)
{
    static constexpr A a1{1};
    static constexpr A a2{2};
    static constexpr B b3{3, 4};
    static constexpr CompileTimeList list{A{1}, A{2}, B{3, 4}};

    // will call the const functions, because `list` may not change.
    EXPECT_EQ(a1.member1, list.get().member1);
    EXPECT_EQ(a2.member1, list.next().get().member1);
    EXPECT_EQ(b3.member1, list.next().next().get().member1);
    EXPECT_EQ(b3.member2, list.next().next().get().member2);

    // will call the non-const functions
    auto listCopy = list;
    EXPECT_EQ(a1.member1, listCopy.get().member1);
    EXPECT_EQ(a2.member1, listCopy.next().get().member1);
    EXPECT_EQ(b3.member1, listCopy.next().next().get().member1);
    EXPECT_EQ(b3.member2, listCopy.next().next().get().member2);
}

TEST(CompileTimeList, templatedIndexAccessElements)
{
    static constexpr A a1{1};
    static constexpr A a2{2};
    static constexpr B b3{3, 4};
    static constexpr CompileTimeList list{A{1}, A{2}, B{3, 4}};

    // will call the const functions, because `list` may not change.
    EXPECT_EQ(a1.member1, list.get<0>().member1);
    EXPECT_EQ(a2.member1, list.get<1>().member1);
    EXPECT_EQ(b3.member1, list.get<2>().member1);
    EXPECT_EQ(b3.member2, list.get<2>().member2);

    // will call the non-const functions
    auto listCopy = list;
    EXPECT_EQ(a1.member1, listCopy.get<0>().member1);
    EXPECT_EQ(a2.member1, listCopy.get<1>().member1);
    EXPECT_EQ(b3.member1, listCopy.get<2>().member1);
    EXPECT_EQ(b3.member2, listCopy.get<2>().member2);

    // listCopy.get<3>(); // Index out of bounds, compile-checked
}

TEST(CompileTimeList, runtimeIndexWithMemberVariant)
{
    // This kind of access should be handled carefully.
    // Although this is `constexpr`, we now move in the realm
    // of bounded O(n) access if the compiler decides
    // to delay calculation to run-time.
    static constexpr CompileTimeList list{A{1}, A{2}, B{3, 4}};

    static constexpr std::array<bool, list.size()> hasA = {true, true, false};

    for (size_t i = 0; i < list.size(); i++)
    {
        EXPECT_EQ(std::holds_alternative<A>(list.get(i)), hasA[i]);
    }

    // intentionally accessing over boundaries:
    static constexpr bool hasNothingWhenReadingOverBoundaries =
            std::holds_alternative<Nothing>(list.get(list.size()));
    EXPECT_TRUE(hasNothingWhenReadingOverBoundaries);
}

TEST(CompileTimeList, getConstList)
{
    CompileTimeList list{A{1}, A{2}, B{3, 4}};

    auto constList = list.asConstList();

    using NormalListType = decltype(list);
    using ConstListType = decltype(constList);

    static constexpr bool normalIsMutable =
            std::is_same_v<NormalListType, CompileTimeList<A, A, B>>;
    // with `asConstList`, the Info about ReferenceList or not degrades.
    // but it still holds references, so it's ok
    static constexpr bool constIsUnMutable =
            std::is_same_v<ConstListType, CompileTimeList<const A, const A, const B>>;

    static_assert(normalIsMutable);
    static_assert(constIsUnMutable);

    // just to add unnecessary test for completeness:

    list.get().member1++;  // possible
    // constList.get().member1++;   // not possible

    static constexpr bool nonConstAIsNonConst = std::is_same_v<decltype(list.get()), A&>;
    static constexpr bool constAIsConst = std::is_same_v<decltype(constList.get()), const A&>;

    static_assert(nonConstAIsNonConst);
    static_assert(constAIsConst);
}

TEST(ReferenceCompileTimeList, canBeInstantiated)
{
    static constexpr A a1{1};
    static constexpr A a2{2};
    static constexpr B b3{1, 2};
    static constexpr ReferenceList list{a1, a2, b3};
    (void) list;
}

TEST(ReferenceCompileTimeList, hasCorrectlyTypedReferences)
{
    A a1{1};
    const A a2{2};  // this one is const!
    B b3{3, 4};
    ReferenceList list{a1, a2, b3};

    EXPECT_EQ(a1.member1, list.get().member1);
    EXPECT_EQ(a2.member1, list.next().get().member1);
    EXPECT_EQ(b3.member1, list.next().next().get().member1);
    EXPECT_EQ(b3.member2, list.next().next().get().member2);

    // now change the original values
    a1.member1 = 11;
    // a2.member1 = 12; // we can't change a2, because it is const
    b3.member1 = 13;
    b3.member2 = 14;

    EXPECT_EQ(a1.member1, list.get().member1);
    EXPECT_EQ(a2.member1, list.next().get().member1);
    EXPECT_EQ(b3.member1, list.next().next().get().member1);
    EXPECT_EQ(b3.member2, list.next().next().get().member2);

    // now change from the list
    // (possible even though the operators return a const reference!)
    list.get().member1++;
    // list.next().get().member1++;    // we can't change a2, because it is const
    static_assert(std::is_same_v<decltype(list.next().get()), const A&>,
                  "a2 is const, compile-time checked!");
    list.next().next().get().member1++;
    list.next().next().get().member2++;

    // one more, individually
    EXPECT_EQ(a1.member1, 12);
    EXPECT_EQ(a2.member1, 2);
    EXPECT_EQ(b3.member1, 14);
    EXPECT_EQ(b3.member2, 15);

    // access by non-const reference matching
    auto& a1Ref = list.get();
    a1Ref = A{20};
    EXPECT_EQ(a1.member1, 20);
}

TEST(CompileTimeList, accessMembersTypesCompileTime)
{
    // holds its own data
    static constexpr CompileTimeList list{A{1}, A{2}, B{3, 4}};

    static constexpr bool hasListFirst = list.hasNext();
    static_assert(hasListFirst, "list has wrongly no first element, compile-time checked!");

    static constexpr auto& first = list.get();
    static constexpr bool isFirstOfAType = std::is_same_v<decltype(first), const A&>;
    static constexpr bool isFirstOfBType = std::is_same_v<decltype(first), const B&>;
    static_assert(isFirstOfAType, "first is wrongly 'not A-Type', compile-time checked!");
    static_assert(!isFirstOfBType, "first is wrongly 'B-Type', compile-time checked!");

    static constexpr bool hasListSecond = list.hasNext();
    static_assert(hasListSecond, "list has wrongly no second element, compile-time checked!");

    static constexpr auto& second = list.next().get();
    static constexpr bool isSecondOfAType = std::is_same_v<decltype(second), const A&>;
    static constexpr bool isSecondOfBType = std::is_same_v<decltype(second), const B&>;
    static_assert(isSecondOfAType, "second is not A-Type, compile-time checked!");
    static_assert(!isSecondOfBType, "second is wrongly 'B-Type', compile-time checked!");

    static constexpr bool hasListThird = list.next().hasNext();
    static_assert(hasListThird, "list has wrongly no third element, compile-time checked!");

    static constexpr auto& third = list.next().next().get();
    static constexpr bool isThirdOfAType = std::is_same_v<decltype(third), const A&>;
    static constexpr bool isThirdOfBType = std::is_same_v<decltype(third), const B&>;
    static_assert(!isThirdOfAType, "third is wrongly 'A-Type', compile-time checked!");
    static_assert(isThirdOfBType, "third is wrongly 'not B-Type', compile-time checked!");

    static constexpr bool hasListFourth = list.next().next().hasNext();
    static_assert(!hasListFourth, "list has wrongly a fourth element, compile-time checked!");

    static constexpr auto& end = list.next().next().next();
    static constexpr bool isEndActualEndType = std::is_same_v<decltype(end), const EmptyList&>;
    static_assert(isEndActualEndType, "List has wrongly more elements, compile-time checked!");

    // The following (moving past the end of list) is not possible,
    // because it is known at compile-time.

    // static constexpr auto fourth = list.next().next().next().get();
}

// ----------------------------------------------------------------------------
//
//  Real world example: One function to access the common members (Quack!)
//
// ----------------------------------------------------------------------------

/**
 * @brief The End-Function to stop the recursion.
 *
 * It is needed to be defined, even though we might not actually call it
 * (perhaps guarded by `CompileTimeList::hasNext()`). But even with an `if constexpr ()`,
 * The code needs to be compile_able_. So we need it in any case
 * (or call piep_pc if you know an alternative!)
 */
static void
printElement1(EmptyList /* empty */)
{
    std::cout << std::endl;
}

template <typename... ListElements>
void
printElement1(const CompileTimeList<ListElements...>& list)
{
    const auto& element = list.get();

    std::cout << element.member1;

    if (list.hasNext())
    {
        std::cout << " ";
    }
    printElement1(list.next());
}

TEST(CompileTimeList, accessRecursivelySimple)
{
    static constexpr CompileTimeList list{A{1}, A{2}, B{3, 4}, A{5}};

    testing::internal::CaptureStdout();
    printElement1(list);
    auto log = testing::internal::GetCapturedStdout();

    EXPECT_THAT(log, testing::StrCaseEq("1 2 3 5\n"));  // note the "missing" 4 of B.member2
}

// ----------------------------------------------------------------------------
//
//  Real world example 2: Recursive dispatcher that uses overloaded functions
//
// ----------------------------------------------------------------------------

// The "dispatched" function specializations:

static void
myPrint(const A& a)
{
    std::cout << a.member1;
}

static void
myPrint(const B& b)
{
    std::cout << b.member1 << " " << b.member2;
}

// The "dispatcher":

static void
dispatchPrint(EmptyList /* empty */)
{
    std::cout << std::endl;
}

template <typename... ListElements>
void
dispatchPrint(const CompileTimeList<ListElements...>& list)
{
    const auto& element = list.get();

    myPrint(element);  // The compiler will choose an appropriate overload

    if (list.hasNext())
    {
        std::cout << " ";
    }
    dispatchPrint(list.next());
}

// -----

TEST(CompileTimeList, accessRecursivelyDispatch)
{
    static constexpr CompileTimeList list{A{1}, A{2}, B{3, 4}, A{5}};

    testing::internal::CaptureStdout();
    dispatchPrint(list);
    auto log = testing::internal::GetCapturedStdout();
    EXPECT_THAT(log, testing::StrCaseEq("1 2 3 4 5\n"));  // note the '4' (B::member2)
}
