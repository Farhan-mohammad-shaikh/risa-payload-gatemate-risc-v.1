/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/utils/safe_mask.h>

#include <unittest/harness.h>

#include <stdint.h>

using namespace outpost;

class A : public Mask<A>
{
public:
    constexpr A() = default;
    // conversion-constructor to parent
    // cppcheck-suppress noExplicitConstructor
    constexpr A(const Mask<A>& mask) : Mask<A>(mask){};

    // Values
    static constexpr A
    thing()
    {
        return A{0b001};
    }

    static constexpr A
    otherThing()
    {
        return A{0b010};
    }

private:
    using Mask<A>::Mask;
};

class B : public Mask<B>
{
public:
    constexpr B() = default;
    // conversion-constructor to parent
    // cppcheck-suppress noExplicitConstructor
    constexpr B(const Mask<B>& mask) : Mask<B>(mask){};

    // Values
    static constexpr B
    thing()
    {
        return B{0b001};
    }

    static constexpr B
    otherThing()
    {
        return B{0b010};
    }

private:
    using Mask<B>::Mask;
};

// ---------------------------------------------------------------

// available only from c++17 onwards, so implement our own version
template <typename... Ts>
struct make_void
{
    typedef void type;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

template <class T1, class T2, class R = void>
struct ImplementsOperatorOr
{
    static constexpr bool implements = false;
};

template <class T1, class T2>
struct ImplementsOperatorOr<T1, T2, void_t<decltype(std::declval<T1>() | std::declval<T2>())>>
{
    // cppcheck is confused and does not know that is indeed used
    // cppcheck-suppress unusedStructMember
    static constexpr bool implements =
            std::is_same<typename std::remove_cv<T1>::type,
                         typename std::remove_cv<decltype(std::declval<T1>()
                                                          | std::declval<T2>())>::type>::value;
};

TEST(SafeMask, types)
{
    const auto aThing = A::thing();
    static_assert(std::is_same<decltype(aThing), const A>::value, "A::thing is not of type A");

    const auto bThing = B::thing();
    static_assert(std::is_same<decltype(bThing), const B>::value, "B::thing is not of type B");

    //    const auto notWorking = A::thing() | B::thing();
    const bool canAWithA = ImplementsOperatorOr<A, A>::implements;
    const bool canAWithB = ImplementsOperatorOr<A, B>::implements;
    EXPECT_TRUE(canAWithA);
    EXPECT_FALSE(canAWithB);

    const bool canInstanceAwithA =
            ImplementsOperatorOr<decltype(aThing), decltype(A::otherThing())>::implements;
    const bool canInstanceAwithB =
            ImplementsOperatorOr<decltype(aThing), decltype(bThing)>::implements;
    EXPECT_TRUE(canInstanceAwithA);
    EXPECT_FALSE(canInstanceAwithB);
}

TEST(SafeMask, hasSet)
{
    const auto thingHasThing = A::thing().isSet(A::thing());
    EXPECT_TRUE(thingHasThing);
    const auto thingHasOtherThing = A::thing().isSet(A::otherThing());
    EXPECT_FALSE(thingHasOtherThing);
}

TEST(SafeMask, operatorOr)
{
    EXPECT_EQ(A::thing() | A::thing(), A::thing());
    EXPECT_NE(A::thing() | A::otherThing(), A::thing());
    const auto both = A::thing() | A::otherThing();
    EXPECT_TRUE(both.isSet(A::thing()) && both.isSet(A::otherThing()));
}

TEST(SafeMask, operatorAnd)
{
    EXPECT_EQ(A::thing() & A::thing(), A::thing());
    EXPECT_NE(A::thing() & A::otherThing(), A::thing());
    EXPECT_EQ(A::thing() & A::otherThing(), A());  // nothing
}
