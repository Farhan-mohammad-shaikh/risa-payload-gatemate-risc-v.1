/*
 * Copyright (c) 2023-2024, Pascal Pieper
 * Copyright (c) 2023-2024, Jan-Gerd Mess
 * Copyright (c) 2023-2024, Pieper, Pascal
 * Copyright (c) 2023-2024, Janosch Reinking
 * Copyright (c) 2023, Kirstein
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/utils/expected.h>

#include <unittest/harness.h>

#include <functional>
#include <type_traits>

namespace testing
{
struct ReturnType
{
    int someMember;

    bool
    // cppcheck-suppress functionStatic
    constFunctionReturningTrue() const
    {
        return true;
    }

    void
    nonConstFunction()
    {
        someMember++;
        someMember--;
    }
};
enum class ErrorType
{
    some,
    error,
    conditions
};
}  // namespace testing

TEST(Unexpected, BasicMemberExtraction)
{
    auto e_rvalue_error = outpost::unexpected(testing::ErrorType::error).error();
    EXPECT_EQ(e_rvalue_error, testing::ErrorType::error);

    auto e = outpost::unexpected(testing::ErrorType::error);
    ASSERT_EQ(e, outpost::unexpected(testing::ErrorType::error));
    ASSERT_FALSE(e == outpost::unexpected(testing::ErrorType::some));
}

TEST(Expected, Observers)
{
    using Result = outpost::Expected<testing::ReturnType, testing::ErrorType>;
    static_assert(std::is_same<Result::value_t, testing::ReturnType>::value,
                  "Incorrect value type deduced");
    static_assert(std::is_same<Result::error_t, testing::ErrorType>::value,
                  "Incorrect return type deduced");
    static_assert(
            std::is_same<Result::unexpected_t, outpost::Unexpected<testing::ErrorType>>::value,
            "Incorrect unexpected construct type deduced");

    const auto expectedValue = testing::ReturnType{12};
    Result successfulResult = expectedValue;
    const Result constSuccess = successfulResult;
    Result errorResult = outpost::unexpected(testing::ErrorType::error);

    // has_value() observers
    EXPECT_TRUE(successfulResult);
    EXPECT_TRUE(successfulResult.has_value());
    EXPECT_FALSE(errorResult);
    EXPECT_FALSE(errorResult.has_value());

    // value() observers
    {
        // l-value reference
        EXPECT_EQ(successfulResult->someMember, expectedValue.someMember);
        EXPECT_EQ(constSuccess->someMember, expectedValue.someMember);
        successfulResult->nonConstFunction();
        // TODO: How to assert that this does not compile?
        // constSuccess->nonConstFunction();
        EXPECT_TRUE(constSuccess->constFunctionReturningTrue());
    }
    {
        // r-value reference (no move)
        EXPECT_EQ(
                Result { expectedValue } -> someMember, expectedValue.someMember);
        // FIXME the following uglyness is due to outdated clang-format on CI runner
        Result
        {
            expectedValue
        } -> nonConstFunction();
        // cRef.nonConstFunction();
        EXPECT_TRUE(Result { expectedValue } -> constFunctionReturningTrue());
    }
    {
        // l-value reference
        auto& ref = *successfulResult;
        auto& cRef = *constSuccess;
        EXPECT_EQ(ref.someMember, expectedValue.someMember);
        EXPECT_EQ(cRef.someMember, expectedValue.someMember);
        ref.nonConstFunction();
        // cRef.nonConstFunction();
        EXPECT_TRUE(cRef.constFunctionReturningTrue());
    }
    {
        // r-value reference (move)
        auto ref = *(Result{expectedValue});
        const auto cRef = *(Result{expectedValue});
        EXPECT_EQ(ref.someMember, expectedValue.someMember);
        EXPECT_EQ(cRef.someMember, expectedValue.someMember);
        ref.nonConstFunction();
        // cRef.nonConstFunction();
        EXPECT_TRUE(cRef.constFunctionReturningTrue());
    }
    {
        // l-value
        const auto& ref = errorResult.error();
        const auto& cRef = errorResult.error();
        ASSERT_EQ(ref, testing::ErrorType::error);
        ASSERT_EQ(cRef, testing::ErrorType::error);
    }
    {
        // r-value
        auto ref = Result{outpost::unexpected(testing::ErrorType::error)}.error();
        const auto cRef = Result{outpost::unexpected(testing::ErrorType::error)}.error();
        ASSERT_EQ(ref, testing::ErrorType::error);
        ASSERT_EQ(cRef, testing::ErrorType::error);
    }

    EXPECT_EQ(errorResult.error(), testing::ErrorType::error);
}

template <typename ReturnType, typename ErrorType>
outpost::Expected<typename std::remove_const<ReturnType>::type,
                  typename std::remove_const<ErrorType>::type>
returnSuccessfulOrNot(bool returnSuccessful, ReturnType val, ErrorType err)
{
    if (returnSuccessful)
    {
        return val;
    }
    else
    {
        return outpost::unexpected(err);
    }
}

TEST(Expected, ReturningFunction)
{
    EXPECT_TRUE(returnSuccessfulOrNot(true, testing::ReturnType{12}, testing::ErrorType::some));

    {
        // const value
        const auto ret = testing::ReturnType{12};
        EXPECT_TRUE(returnSuccessfulOrNot(true, ret, testing::ErrorType::some).has_value());
        EXPECT_FALSE(returnSuccessfulOrNot(false, ret, testing::ErrorType::some).has_value());
    }

    {
        // const function (and pointer value)
        const auto ret = returnSuccessfulOrNot(true, "const string", testing::ErrorType::some);
        EXPECT_TRUE(ret.has_value());
        EXPECT_TRUE(strcmp(*ret, "const string") == 0);
    }
}

TEST(Expected, SameValueAndErrorType)
{
    auto succ = returnSuccessfulOrNot(true, 15, 20);
    auto erro = returnSuccessfulOrNot(false, 15, 20);
    const auto succConst = succ;
    const auto erroConst = erro;
    auto& succRef = succ;
    auto& erroRef = erro;
    const auto& succConstRef = succ;
    const auto& erroConstRef = erro;

    EXPECT_TRUE(succ);
    EXPECT_EQ(*succ, 15);
    EXPECT_FALSE(erro);
    EXPECT_EQ(erro.error(), 20);

    EXPECT_TRUE(succConst);
    EXPECT_EQ(*succConst, 15);
    EXPECT_FALSE(erroConst);
    EXPECT_EQ(erroConst.error(), 20);

    EXPECT_TRUE(succRef);
    EXPECT_EQ(*succRef, 15);
    EXPECT_FALSE(erroRef);
    EXPECT_EQ(erroRef.error(), 20);

    EXPECT_TRUE(succConstRef);
    EXPECT_EQ(*succConstRef, 15);
    EXPECT_FALSE(erroConstRef);
    EXPECT_EQ(erroConstRef.error(), 20);

    const auto&& succConstDRef = std::move(succ);
    const auto&& erroConstDRef = std::move(erro);

    EXPECT_TRUE(succConstDRef);
    EXPECT_EQ(*succConstDRef, 15);
    EXPECT_FALSE(erroConstDRef);
    EXPECT_EQ(erroConstDRef.error(), 20);
}

TEST(Expected, DefaultOnErrorConditionWithNonTrivialConstructors)
{
    class NotTrivial
    {
        bool mValue;
        NotTrivial() = delete;

    public:
        explicit NotTrivial(bool val) : mValue(val){};

        bool
        accessMember() const
        {
            return mValue;
        }
    };

    auto succ = returnSuccessfulOrNot(true, NotTrivial{true}, NotTrivial{false});
    auto erro = returnSuccessfulOrNot(false, NotTrivial{true}, NotTrivial{false});

    ASSERT_TRUE(succ);
    ASSERT_FALSE(erro);

    ASSERT_EQ(succ->accessMember(), true);
    ASSERT_EQ(erro.error().accessMember(), false);

    ASSERT_EQ(succ.value_or(NotTrivial{false}).accessMember(), true);
    ASSERT_EQ(erro.value_or(NotTrivial{true}).accessMember(), true);
}

#if defined(OUTPOST_USE_ASSERT) && OUTPOST_USE_ASSERT == 1
TEST(Expected, AssertionsWorking)
{
    auto succ = returnSuccessfulOrNot(true, true, false);
    auto erro = returnSuccessfulOrNot(false, true, false);
    bool shouldfail;

    EXPECT_DEATH({ shouldfail = *erro; }, "Invalid access to expected value");
    (void) shouldfail;  // "use" variable
    EXPECT_DEATH({ shouldfail = succ.error(); }, "Invalid access to error value");
    (void) shouldfail;  // "use" variable
}
#else
#pragma message("Building without activated assertions, skipping death test")
#endif

TEST(Expected, Constructors)
{
    using Result = outpost::Expected<int, int>;
    using DifferentResult = outpost::Expected<double, double>;

    {
        // Default constructor
        Result res;
        ASSERT_TRUE(res);

        // lvalue copy constructor
        Result res2{res};
        ASSERT_TRUE(res2);
    }

    {
        // l-value constructor
        Result res{true};
        ASSERT_TRUE(res);
    }

    {
        // lvalue error type
        Result res{outpost::unexpected(-1)};
        ASSERT_FALSE(res);
        EXPECT_EQ(res.error(), -1);
    }

    {
        // lvalue error type, inline conversion
        Result res{outpost::unexpected(+1)};
        ASSERT_FALSE(res);
        EXPECT_EQ(res.error(), +1);
    }

    {
        // rvalue copy constructor
        Result res{Result{400}};
        ASSERT_TRUE(res);
        EXPECT_EQ(*res, 400);
    }

    {
        // lvalue expected inline conversion from success
        DifferentResult dres{1.5};
        Result res{dres};
        ASSERT_TRUE(res);
        EXPECT_EQ(*res, 1);
    }

    {
        // lvalue expected inline conversion from error
        DifferentResult dres{outpost::unexpected(-1.5)};
        Result res{dres};
        ASSERT_FALSE(res);
        EXPECT_EQ(res.error(), -1);
    }

    {
        // rvalue expected inline conversion
        Result res{DifferentResult{1.5}};
        ASSERT_TRUE(res);
        EXPECT_EQ(*res, 1);
    }

    {
        // rvalue expected inline conversion from error
        Result res{DifferentResult{outpost::unexpected(1.5)}};
        ASSERT_FALSE(res);
        EXPECT_EQ(res.error(), 1);
    }
}

TEST(Expected, AssignmentOperators)
{
    struct Track
    {
        explicit Track(std::function<void()> destructorCallback) :
            mDestructorCallback(destructorCallback){};

        ~Track()
        {
            if (mDestructorCallback)
            {
                mDestructorCallback();
            }
        }

        std::function<void()> mDestructorCallback;
    };

    using DefaultResult = outpost::Expected<int, int>;
    using DifferentResult = outpost::Expected<double, double>;
    using TrackingResult = outpost::Expected<Track, Track>;

    auto maybe = returnSuccessfulOrNot(true, 1, 1);
    ASSERT_TRUE(maybe);

    static_assert(std::is_same<DefaultResult, decltype(maybe)>::value, "Assumption does not hold");

    // defined here to allow modifications after (stack) destruction
    bool wasValueDestructed;

    {
        // l-value assignment, from expected to unexpected
        wasValueDestructed = false;

        TrackingResult res{Track{[&wasValueDestructed](void) { wasValueDestructed = true; }}};
        TrackingResult other{outpost::unexpected(Track{[](void) { ; }})};

        ASSERT_TRUE(res);
        ASSERT_FALSE(other);

        res = other;
        EXPECT_TRUE(wasValueDestructed);
        ASSERT_FALSE(res);
    }

    {
        // l-value assignment, from unexpected to expected
        wasValueDestructed = false;

        TrackingResult res{outpost::unexpected(Track{[](void) { ; }})};
        TrackingResult other{Track{[&wasValueDestructed](void) { wasValueDestructed = true; }}};

        ASSERT_FALSE(res);
        ASSERT_TRUE(other);

        res = other;
        EXPECT_TRUE(wasValueDestructed);
        ASSERT_TRUE(res);
    }

    {
        // r-value assignment
        TrackingResult res{outpost::unexpected(Track{[](void) { ; }})};
        ASSERT_FALSE(res);

        // from unexpected to expected
        wasValueDestructed = false;
        res = TrackingResult{Track{[&wasValueDestructed](void) { wasValueDestructed = true; }}};
        ASSERT_TRUE(res);
        EXPECT_TRUE(wasValueDestructed);

        // from expected to unexpected
        wasValueDestructed = false;
        res = TrackingResult{outpost::unexpected(
                Track{[&wasValueDestructed](void) { wasValueDestructed = true; }})};
        ASSERT_FALSE(res);
        EXPECT_TRUE(wasValueDestructed);
    }

    {
        // rvalue "expected value" assignment inline conversion
        DefaultResult res{outpost::unexpected(false)};
        res = 1;
        ASSERT_TRUE(res);
        EXPECT_EQ(*res, true);
    }

    {
        // lvalue different class inline conversion
        DefaultResult res;
        DifferentResult different{1};
        res = different;
        ASSERT_TRUE(res);
        EXPECT_EQ(*res, true);

        different = outpost::unexpected(0);  // converted to "DifferentResult"
        res = different;
        ASSERT_FALSE(res);
        EXPECT_EQ(res.error(), false);
    }

    {
        // lvalue "unexpected" assingment inline conversion
        DefaultResult res{1};
        const auto unex = outpost::unexpected(0);
        res = unex;
        ASSERT_FALSE(res);
        EXPECT_EQ(res.error(), 0);
    }

    {
        // rvalue "unexpected" assingment inline conversion
        maybe = outpost::unexpected(2);
        ASSERT_FALSE(maybe);
        EXPECT_EQ(maybe.error(), 2);
    }
}

#if __cplusplus < 201702L
namespace std
{
// available only from c++17 onwards, so implement our own version

template <typename... Ts>
struct make_void
{
    typedef void type;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;
}  // namespace std
#endif

template <class T, typename = void>
struct has_bool_operator
{
    static constexpr bool value = false;
};

template <class T>
struct has_bool_operator<T, std::void_t<decltype(std::declval<T&>().operator bool())>>
{
    // cppcheck-suppress unusedStructMember
    static constexpr bool value = true;
};

TEST(Expected, booleanTypeCheck)
{
    struct NoBoolOperator
    {
    };

    struct WithBoolOperator
    {
        operator bool() const
        {
            return true;
        }
    };

    const bool hasOperatorWithThing =
            has_bool_operator<outpost::Expected<NoBoolOperator, NoBoolOperator>>::value;
    EXPECT_TRUE(hasOperatorWithThing);

    const bool hasNoOperatorWithBool =
            !has_bool_operator<outpost::Expected<bool, NoBoolOperator>>::value;
    EXPECT_TRUE(hasNoOperatorWithBool);

    const bool hasNoOperatorWithExplicitBoolOperator =
            !has_bool_operator<outpost::Expected<WithBoolOperator, NoBoolOperator>>::value;
    EXPECT_TRUE(hasNoOperatorWithExplicitBoolOperator);

    const bool hasOperatorWithImplicitBoolConversion =
            has_bool_operator<outpost::Expected<int, NoBoolOperator>>::value;
    EXPECT_TRUE(hasOperatorWithImplicitBoolConversion);
}
