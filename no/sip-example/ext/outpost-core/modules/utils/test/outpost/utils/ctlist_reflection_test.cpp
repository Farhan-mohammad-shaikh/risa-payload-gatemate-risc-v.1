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

#include <outpost/utils/ctlist_reflection.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <inttypes.h>

#include <iostream>

using namespace outpost;
using namespace outpost::reflection;

using outpost::container::CompileTimeList;
using outpost::container::ReferenceList;

using reflection::detail::implements_any_access_v;
using reflection::detail::ImplementsNonConstAccess;

// ----------------------------------------------------------------------------
// Example visitable Elements.
// Requirement: implement `ReferenceList<...> getMember()`
// ----------------------------------------------------------------------------

/// Struct with a single member.
struct OneMember
{
    int member1;

    constexpr auto
    getMember()
    {
        return ReferenceList{member1};
    }
};

// Actually directly a test
static_assert(ImplementsNonConstAccess<OneMember>::value, "should implement getMember()");

/// struct with two members.
struct TwoMember
{
    OneMember member1;
    OneMember member2;

    constexpr auto
    getMember()
    {
        return ReferenceList{member1, member2};
    }
};

static_assert(ImplementsNonConstAccess<TwoMember>::value, "should implement getMember()");

/**
 * Struct with differently typed, plain, members
 * Also it does not implement `getMember()`, demonstrating
 * a custom solution. Note that with this, every Visitor
 * needs a special function or only calls functions that
 * are defined on this struct anyways
 * (like the following `operator<<()`)
 */
struct NoDirectMemberAccess
{
    float member1;
    uint16_t member2;

    // No `getMember()`!
    // simulates a custom solution.
    // As it implements the `operator<<`, this will be taken by
    // the `PrintVisitor`.
    // *Other Visitors will have to implement their own overloaded function.*
};

static_assert(!implements_any_access_v<NoDirectMemberAccess>, "does not implement getMember()");

/**
 * @brief Custom overload for printing `NoDirectMemberAccess`.
 *        The intention here is showing that the new approach
 *        is still compatible to the old `Serialize.store<>(C)`.
 */
static std::ostream&
operator<<(std::ostream& os, const NoDirectMemberAccess& c)
{
    os << c.member1 << "custom" << c.member2;
    return os;
}

/**
 * A struct with mixed "visitable" and plain types, to demonstrate
 * that new and old types can be mixed without having to know what kind
 * of members they actually are.
 */
struct MixedAccessTypes
{
    OneMember a;
    TwoMember b;
    NoDirectMemberAccess c;
    int member4;

    constexpr auto
    getMember()
    {
        return ReferenceList{a, b, c, member4};
    }
};

static_assert(ImplementsNonConstAccess<MixedAccessTypes>::value, "should implement getMember()");

// ----------------------------------------------------------------------------
// Example Visitors (can be De/Serializer, printer, ...)
// Requirement: Implement `void operator(element)`.
// Optionally also `void operator(element, hasNext)` or `void operator(void)`.
// ----------------------------------------------------------------------------

/// Example Visitor that only reads from the elements
struct PrintVisitor
{
    bool withNewline = false;

    /**
     * Print operator that takes optional parameter `hasNext`
     * To infer knowledge where to put the space between the elements.
     * This second parameter is optional, though.
     */
    template <typename T>
    void
    operator()(const T& element, bool hasNext) const
    {
        std::cout << element;
        if (hasNext)
            std::cout << " ";
    }

    /**
     * Print operator that does not tok optional parameter `hasNext`.
     * It won't be called as long as the function *with* bool is defined.
     */
    template <typename T>
    void
    operator()(const T&) const
    {
        // This will not be called, because the `operator(T, hasNext)` is preferred!
        assert(false);
    }

    // Note: No overloaded operator()(const C& ...)
    // because it uses the custom `operator<<(..., C)` above!

    // This will be called at the end after all elements.
    // Implementation is optional, and can also be implemented using
    // `!hasNext` in the `operator()(const T& element, bool hasNext)`.
    void
    operator()() const
    {
        if (withNewline)
            std::cout << std::endl;
    }
};

/// Example Visitor that modifies the elements
struct IncrementVisitor
{
    /**
     * Calls the `operator++` on generic types.
     * Does not care about whether there are next elements.
     */
    template <typename T>
    constexpr void
    operator()(T& element) const
    {
        element++;
    }

    // Special implementation for struct `NoDirectMemberAccess`
    // (which does not have an overloaded `operator++`)
    // to demonstrate custom solutions
    constexpr void
    operator()(NoDirectMemberAccess& element) const
    {
        element.member1 += 1.5;
        element.member2++;
    }
};

/// Example Visitor that adds all elements together
struct AccumulateVisitor
{
    double sum = 0;

    // non-const function that does not change the element
    template <typename T>
    constexpr void
    operator()(const T& element)
    {
        sum += element;
    }

    /*
     * We need to either implement the function, or ignore all `T`s
     * that don't have an `operator+=()` in the generic type above.
     * this is because this type chose to not implement `getMember()`
     * but is also not fundamental, so no default operations exist.
     */
    constexpr void
    operator()(const NoDirectMemberAccess& element)
    {
        sum += element.member1;
        sum += element.member2;
    }
};

// ----------------------------------------------------------------------------
// Actual Tests
// ----------------------------------------------------------------------------

TEST(CompileTimeListReflections, printConstAndNonConstElement)
{
    static constexpr OneMember a{1};
    static constexpr PrintVisitor printer;

    testing::internal::CaptureStdout();
    visit(a, printer);
    auto outputFromConst = testing::internal::GetCapturedStdout();
    EXPECT_EQ(outputFromConst, "1");

    OneMember aMut = a;
    aMut.member1 = rand();

    testing::internal::CaptureStdout();
    visit(aMut, printer);
    auto outputFromMutable = testing::internal::GetCapturedStdout();
    EXPECT_EQ(outputFromMutable, std::to_string(aMut.member1));
}

TEST(CompileTimeListReflections, printNestedStaticElement)
{
    static constexpr TwoMember ab{OneMember{1}, OneMember{2}};
    static constexpr PrintVisitor printer;

    testing::internal::CaptureStdout();
    visit(ab, printer);
    auto output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "1 2");
}

TEST(CompileTimeListReflections, printFromMixedConstOnlyAccess)
{
    struct OnlyConstAccess
    {
        OneMember a;

        constexpr auto
        getMember() const  // This 'const' will prevent any modifying access.
        {
            return ReferenceList{a};
        }
    };

    static constexpr PrintVisitor printer;
    OnlyConstAccess o{OneMember{1}};

    testing::internal::CaptureStdout();
    visit(o, printer);
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "1");

    // A modifying visitor will not work:
    // static constexpr IncrementVisitor incrementer;
    // visit(o, incrementer);
}

TEST(CompileTimeListReflections, mixedVisitorAndOverloadedAccess)
{
    // This simulates the situation where there are still
    // types which are defined through the De/Serialize
    // overloaded functions.

    static constexpr MixedAccessTypes advanced{
            OneMember{1}, TwoMember{OneMember{2}, OneMember{3}}, NoDirectMemberAccess{4, 5}, 6};

    static constexpr PrintVisitor printer;

    testing::internal::CaptureStdout();
    visit(advanced, printer);
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "1 2 3 4custom5 6");
}

TEST(CompileTimeListReflections, temporaryVisitor)
{
    static constexpr MixedAccessTypes advanced{
            OneMember{1}, TwoMember{OneMember{2}, OneMember{3}}, NoDirectMemberAccess{4, 5}, 6};

    testing::internal::CaptureStdout();
    visit(advanced, PrintVisitor{});
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "1 2 3 4custom5 6");
}

TEST(CompileTimeListReflections, modifyingVisitor)
{
    // can't be const!
    MixedAccessTypes advanced{
            OneMember{1}, TwoMember{OneMember{2}, OneMember{3}}, NoDirectMemberAccess{4, 5}, 6};

    // Initial:
    testing::internal::CaptureStdout();
    visit(advanced, PrintVisitor{});
    auto preout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(preout, "1 2 3 4custom5 6");

    // modify according to visitor rules
    visit(advanced, IncrementVisitor{});

    testing::internal::CaptureStdout();
    visit(advanced, PrintVisitor{});
    auto postout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(postout, "2 3 4 5.5custom6 7");
}

TEST(CompileTimeListReflections, MutableVisitor)
{
    static constexpr MixedAccessTypes advanced{
            OneMember{1}, TwoMember{OneMember{2}, OneMember{3}}, NoDirectMemberAccess{4, 5}, 6};

    // Initial:
    testing::internal::CaptureStdout();
    visit(advanced, PrintVisitor{});
    auto preout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(preout, "1 2 3 4custom5 6");

    // modify according to visitor rules
    AccumulateVisitor digitSum;
    visit(advanced, digitSum);

    EXPECT_EQ(digitSum.sum, 21);
}

// ------------ detail

TEST(CompileTimeListReflectionsDetail, TypeOfConstMember)
{
    OneMember a = {0};
    using NonConstT = decltype(reflection::detail::getMaybeConstMember(a));
    static_assert(std::is_same_v<NonConstT, ReferenceList<decltype(a.member1)>>);

    const auto& ac = a;
    using ConstT = decltype(reflection::detail::getMaybeConstMember(ac));
    // Because of a technical detail, const ReferenceLists decay to
    // the underlying Type CompileTimeList<T const&>.
    static_assert(std::is_same_v<ConstT, CompileTimeList<int const&>>);

    static_assert(!std::is_same_v<ConstT, NonConstT>);
}
