/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2023-2024, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

/**
 * \file    harness.h
 * \brief    Test harness
 *
 * \author    Fabian Greif
 */

#ifndef OUTPOST_HARNESS_H
#define OUTPOST_HARNESS_H

#include <outpost/base/slice.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Source: stack overflow, Fraser '12
template <typename T, size_t size>
::testing::AssertionResult
ArraysMatch(const T (&expected)[size], const T (&actual)[size])
{
    for (size_t i(0); i < size; ++i)
    {
        if (expected[i] != actual[i])
        {
            return ::testing::AssertionFailure()
                   << "array[" << i << "] (" << actual[i] << ") != expected[" << i << "] ("
                   << expected[i] << ")";
        }
    }

    return ::testing::AssertionSuccess();
}

template <typename T>
::testing::AssertionResult
ArraysMatch(const T* expected, const T* actual, size_t size)
{
    for (size_t i(0); i < size; ++i)
    {
        if (expected[i] != actual[i])
        {
            return ::testing::AssertionFailure()
                   << "array[" << i << "] (" << actual[i] << ") != expected[" << i << "] ("
                   << expected[i] << ")";
        }
    }

    return ::testing::AssertionSuccess();
}

template <typename T, size_t size>
::testing::AssertionResult
ArraysMatch(const std::array<T, size>& expected, const std::array<T, size>& actual)
{
    for (size_t i = 0; i < size; ++i)
    {
        if (expected[i] != actual[i])
        {
            return ::testing::AssertionFailure()
                   << "array[" << i << "] (" << actual[i] << ") != expected[" << i << "] ("
                   << expected[i] << ")";
        }
    }

    return ::testing::AssertionSuccess();
}

template <typename T1,
          typename T2,
          class = std::enable_if_t<std::is_same_v<std::remove_cv_t<T1>, std::remove_cv_t<T2>>>>
::testing::AssertionResult
SliceMatch(const outpost::Slice<T1>& expected, const outpost::Slice<T2>& actual)
{
    if (expected.getNumberOfElements() != actual.getNumberOfElements())
    {
        return ::testing::AssertionFailure()
               << "Slices length do not match: "
               << "expected: " << expected.getNumberOfElements() << ", "
               << "actual: " << actual.getNumberOfElements();
    }
    for (size_t i = 0; i < expected.getNumberOfElements(); i++)
    {
        if (expected[i] != actual[i])
        {
            return ::testing::AssertionFailure() << "Slice content does not match: "
                                                    "expected["
                                                 << i << "] (" << expected[i] << "), "
                                                 << "actual[" << i << "] (" << actual[i] << ")";
        }
    }
    return ::testing::AssertionSuccess();
}

::testing::AssertionResult
StringsMatch(const char* a, const char* b);

/**
 * Compare two arrays.
 * @warning DEPRECATED now, use the ::testing version
 *
 * Example:
 *   uint8_t expect[16];
 *   uint8_t result[16];
 *   ...
 *   EXPECT_ARRAY_EQ(uint8_t, expect, result, 16);
 */
#define EXPECT_ARRAY_EQ(type, reference, actual, element_count)       \
    do                                                                \
    {                                                                 \
        const type* reference_ = static_cast<const type*>(reference); \
        const type* actual_ = static_cast<const type*>(actual);       \
        for (size_t i = 0; i < element_count; ++i)                    \
        {                                                             \
            EXPECT_EQ(reference_[i], actual_[i]);                     \
        }                                                             \
    } while (0)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/**
 * Compare two slices.
 *
 * Example:
 *   uint8_t expect[16];
 *   uint8_t result[16];
 *   ...
 *   EXPECT_SLICE_EQ(asSlice(expect), asSlice(result));
 */
#define EXPECT_SLICE_EQ(sliceA, sliceB)                                                         \
    do                                                                                          \
    {                                                                                           \
        EXPECT_EQ(typeid(typename std::remove_cv<typename decltype(sliceA)::value_type>::type), \
                  typeid(typename std::remove_cv<typename decltype(sliceB)::value_type>::type)) \
                << "Slices are not of same type";                                               \
        EXPECT_EQ(sliceA.getNumberOfElements(), sliceB.getNumberOfElements())                   \
                << "Slices are not of equal length";                                            \
        for (size_t i = 0; i < sliceA.getNumberOfElements(); i++)                               \
        {                                                                                       \
            EXPECT_EQ(sliceA[i], sliceB[i]) << "Slice element " << i << " not equal";           \
        }                                                                                       \
    } while (0)

#if defined(OUTPOST_USE_ASSERT) && OUTPOST_USE_ASSERT == 1
/**
 * Wrapper around EXPECT_DEATH if assertions are enabled.
 */
#define EXPECT_OUTPOST_ASSERTION_DEATH(thing, msg) EXPECT_DEATH(thing, msg)
#else
#define EXPECT_OUTPOST_ASSERTION_DEATH(thing, msg) EXPECT_TRUE(true)
#endif

#if defined(OUTPOST_USE_ASSERT) && OUTPOST_USE_ASSERT == 1 && __cplusplus >= 201402L
/**
 * Wrapper around EXPECT_DEATH if assertions are enabled and >= c++14 is supported.
 */
#define EXPECT_OUTPOST_CONSTEXPR_ASSERTION_DEATH(thing, msg) EXPECT_DEATH(thing, msg)
#else
#define EXPECT_OUTPOST_CONSTEXPR_ASSERTION_DEATH(thing, msg) EXPECT_TRUE(true)
#endif

#endif  // OUTPOST_HARNESS_H
