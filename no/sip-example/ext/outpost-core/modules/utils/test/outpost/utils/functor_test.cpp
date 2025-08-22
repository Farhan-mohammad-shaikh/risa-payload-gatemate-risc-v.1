/*
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023-2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <harness/utils/expected.h>

#include <outpost/utils/functor.h>

#include <unittest/harness.h>

#include <stdint.h>

using namespace outpost;

struct Testclass
{
    bool
    trueFunc()
    {
        ++called;
        return true;
    }

    int
    intPtrFunc(const int* var)
    {
        ++called;
        return *var + 2;
    }

    int
    sumFunc(int a, int b, int c)
    {
        ++called;
        return a + b + c;
    }

    void
    voidOutParamFunc(int* a)
    {
        ++called;
        *a = 42;
    }

    bool
    boolParamFunc(bool someBool)
    {
        ++called;
        return someBool;
    }

    bool
    boolConstRefParamFunc(const bool& someBool)
    {
        ++called;
        return someBool;
    }

    int called = 0;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

TEST(Functor, testLegacy)
{
    int called = 0;

    Testclass testObject;
    EXPECT_EQ(testObject.called, called);

    {
        outpost::Functor<bool()> functor(testObject, &Testclass::trueFunc);
        auto result = functor.execute();
        ASSERT_TRUE(result.has_value());
        EXPECT_TRUE(*result);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<bool()> functor(testObject, &Testclass::trueFunc);
        auto result = functor();
        ASSERT_TRUE(result.has_value());
        EXPECT_TRUE(*result);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<int(const int*)> second(testObject, &Testclass::intPtrFunc);
        int a = 8;
        auto result = second.execute(&a);
        ASSERT_TRUE(result);
        EXPECT_EQ(*result, 10);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<int(int, int, int)> sum(testObject, &Testclass::sumFunc);
        auto result = sum(1, 2, 3);
        ASSERT_TRUE(result);
        EXPECT_EQ(*result, 6);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<int(int, int, int)> invalFunc;
        EXPECT_TRUE(invalFunc.isEmpty());
        EXPECT_EQ(testObject.called, called);
    }

    {
        int a = 0;
        outpost::Functor<void(int*)> voidFunc(testObject, &Testclass::voidOutParamFunc);
        auto result = voidFunc(&a);
        ASSERT_EQ(result, outpost::utils::OperationResult::success);
        EXPECT_EQ(a, 42);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<bool(bool)> boolFunc(testObject, &Testclass::boolParamFunc);
        auto result = boolFunc(true);
        ASSERT_TRUE(result.has_value());
        EXPECT_TRUE(*result);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<bool(bool)> boolFunc(testObject, &Testclass::boolParamFunc);
        bool testBool = false;
        auto result = boolFunc(testBool);
        ASSERT_TRUE(result.has_value());
        EXPECT_FALSE(*result);
        EXPECT_EQ(testObject.called, ++called);
    }
}

#pragma GCC diagnostic pop  // -Wdeprecated-declarations

TEST(Functor, testChecked)
{
    int called = 0;

    Testclass testObject;
    EXPECT_EQ(testObject.called, called);

    {
        outpost::Functor<bool()> functor(testObject, &Testclass::trueFunc);
        auto result = functor.execute();
        ASSERT_TRUE(result.has_value());
        EXPECT_TRUE(*result);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        // operator dispatched by additional arg compared to signature
        outpost::Functor<bool()> functor(testObject, &Testclass::trueFunc);
        auto result = functor();
        ASSERT_TRUE(result.has_value());
        EXPECT_TRUE(*result);
        EXPECT_EQ(testObject.called, ++called);
    }

    outpost::Functor<int(const int*)> second(testObject, &Testclass::intPtrFunc);
    int a = 8;
    {
        auto result = second.execute(&a);
        ASSERT_TRUE(result);
        a = *result;
        EXPECT_EQ(a, 10);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        auto result = second(&a);
        ASSERT_TRUE(result);
        a = *result;
        EXPECT_EQ(a, 12);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<int(int, int, int)> sum(testObject, &Testclass::sumFunc);
        auto result = sum(1, 2, 3);
        ASSERT_TRUE(result);
        EXPECT_EQ(*result, 6);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<int(int, int, int)> invalFunc;
        auto result = invalFunc(1, 2, 3);
        EXPECT_FALSE(result);
        EXPECT_EQ(testObject.called, called);
    }

    {
        outpost::Functor<void(int, int, int)> invalFunc;
        auto result = invalFunc(1, 2, 3);
        EXPECT_FALSE(result != decltype(result)::invalid);
        EXPECT_EQ(testObject.called, called);
    }

    {
        outpost::Functor<void(int*)> voidFunc(testObject, &Testclass::voidOutParamFunc);
        auto result = voidFunc(&a);
        EXPECT_EQ(result, outpost::utils::OperationResult::success);
        EXPECT_EQ(a, 42);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<bool(bool)> boolFunc(testObject, &Testclass::boolParamFunc);
        auto result = boolFunc(true);
        ASSERT_TRUE(result.has_value());
        EXPECT_TRUE(*result);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<bool(bool)> boolFunc(testObject, &Testclass::boolParamFunc);
        bool testBool = false;
        auto result = boolFunc(testBool);
        ASSERT_TRUE(result.has_value());
        EXPECT_FALSE(*result);
        EXPECT_EQ(testObject.called, ++called);
    }

    {
        outpost::Functor<bool(const bool&)> boolFunc(testObject, &Testclass::boolConstRefParamFunc);
        bool testBool = false;
        auto result = boolFunc(testBool);
        ASSERT_TRUE(result.has_value());
        EXPECT_FALSE(*result);
        EXPECT_EQ(testObject.called, ++called);
    }
}
